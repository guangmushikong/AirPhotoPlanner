#include "cateye_mercator.h"

using namespace Cateye::RasterCompiler;

Mercator::Mercator(int tileSize)
{
	_tileSize = tileSize;
	//156543.03392804062 for tileSize 256 pixels
	_initialResolution = 2 * PI * 6378137.0 / _tileSize;
	//# 20037508.342789244
	_originShift = 2 * PI * 6378137.0 / 2.0;
}

Mercator::~Mercator()
{

}

void Mercator::LonLatToMeters(double lon, double lat, double& mx, double& my)
{
	mx = lon * _originShift / 180.0;

	my = log(tan((90.0 + lat) * PI / 360.0 )) / (PI / 180.0);

	my = my * _originShift / 180.0;
	//return mx, my
}

void Mercator::LonLatToTile(double lon, double lat, int zoom, int& tx, int& ty)
{
	double mx = 0, my = 0;
	LonLatToMeters(lon, lat, mx, my);
	MetersToTile(mx, mx, zoom, tx, ty);
}

void Mercator::MetersToLonLat(double mx, double my, double& lon, double& lat)
{
	lon = (mx / _originShift) * 180.0;
	lat = (my / _originShift) * 180.0;

	lat = 180.0 / PI * (2 * atan(exp(lat * PI / 180.0)) - PI / 2.0);
	//return lat, lon
}

void Mercator::PixelsToMeters(double px, double py, int zoom, double& mx,
	double& my)
{
	double res = Resolution(zoom);
	mx = px * res - _originShift;
	my = py * res - _originShift;
	//return mx, my
}

void Mercator::MetersToPixels(double mx, double my, int zoom, double& px, 
	double& py)
{
	double res = Resolution( zoom );
	px = (mx + _originShift) / res;
	py = (my + _originShift) / res;
	//return px, py
}

void Mercator::PixelsToTile(double px, double py, int& tx, int& ty)
{
	//"Returns a tile covering region in given pixel coordinates"

	tx = int(std::ceil(px / double(_tileSize)) - 1);
	ty = int(std::ceil(py / double(_tileSize)) - 1);
	//return tx, ty
}

void Mercator::PixelsToRaster(double px, double py, int zoom, double& rx, 
	double& ry)
{
	//"Move the origin of pixel coordinates to top-left corner"

	int mapSize = _tileSize << zoom;
	rx = px;
	ry = mapSize - py;
	//return px, mapSize - py
}

void Mercator::RasterToPixels(double rx, double ry, int zoom, double& px, 
	double& py)
{
	int mapSize = _tileSize << zoom;
	px = rx;
	py = mapSize - ry;
}

void Mercator::MetersToTile(double mx, double my, int zoom, int& tx, int& ty)
{
	//"Returns tile for given mercator coordinates"
	double px = 0, py = 0;
	MetersToPixels( mx, my, zoom, px, py);
	PixelsToTile(px, py, tx, ty);
	//return self.PixelsToTile( px, py)
}

void Mercator::TileMetersBound(double tx, double ty, int zoom, double bound[])
{
	//"Returns bounds of the given tile in EPSG:900913 coordinates"
	double minx = 0, miny = 0;
	PixelsToMeters(tx * _tileSize, ty * _tileSize, zoom, minx, miny);
	double maxx = 0, maxy = 0;
	PixelsToMeters((tx + 1) * _tileSize, (ty + 1) * _tileSize, zoom, maxx, maxy);
	bound[0] = minx;
	bound[1] = miny;
	bound[2] = maxx;
	bound[3] = maxy;
}

void Mercator::TileLonLatBound(int tx, int ty, int zoom, double bound[])
{
	//"Returns bounds of the given tile in lon/lat using WGS84 datum"
	TileMetersBound(tx, ty, zoom, bound);
	double minLat = 0, minLon = 0;
	MetersToLonLat(bound[0], bound[1], minLon, minLat);

	double maxLat = 0, maxLon = 0;
	MetersToLonLat(bound[2], bound[3], maxLon, maxLat);
	bound[0] = minLon;
	bound[1] = minLat;
	bound[2] = maxLon;
	bound[3] = maxLat;
	//bound = { minLon, minLat, maxLon, maxLat };
}

void Mercator::LonLatBoundToTiles(double lonLatBound[4], int zoom, 
	int tileBound[4])
{
	LonLatToTile(lonLatBound[0], lonLatBound[1], zoom, tileBound[0], 
		tileBound[1]);
	LonLatToTile(lonLatBound[2], lonLatBound[3], zoom, tileBound[2], 
		tileBound[3]);
}

double Mercator::Resolution(int zoom)
{
	//"Resolution (meters/pixel) for given zoom level (measured at Equator)"

	// return (2 * math.pi * 6378137) / (self.tileSize * 2**zoom)
	return _initialResolution / (1 << zoom);
}

int Mercator::ZoomForPixelSize(double pixelSize)
{		
	//"Maximal scaledown zoom of the pyramid closest to the pixelSize."

	for(int i = 0; i < MAXZOOMLEVEL; i++)
	{
		if(pixelSize > Resolution(i))
		{
			if (0 != i)
			{
				/* code */
				return i - 1;
			}
			else
			{
				return 0;
			}
		}
	}
	return 0;
}

void Mercator::GoogleTile(int tx, int ty, int zoom, int& gx, int& gy)
{
	//"Converts TMS tile coordinates to Google Tile coordinates"

	//coordinate origin is moved from bottom-left to top-left corner of the extent
	gx = tx;
	gy = (1 << zoom - 1) - ty;
	//return tx, (2**zoom - 1) - ty
}

std::string Mercator::QuadTree(int tx, int ty, int zoom)
{
	//"Converts TMS tile coordinates to Microsoft QuadTree"

	std::string quadKey = "";
	ty = (1 << zoom - 1) - ty;
	for(int i = zoom; i > 0; i--)
	{
		int digit = 0;

		int mask = 1 << (i - 1);
		if ((tx & mask) != 0)
		{
			digit += 1;
		}		
		if ((ty & mask) != 0)
		{
			digit += 2;
		}
	
		quadKey += to_str(digit);
	}

	return quadKey;
}
