const z64_bgcheck_vertex_t shadowTempleVertexTable[] = {
	{ 0x0190, 0x0000, 0x0000 },
	{ 0xFE70, 0x0000, 0x0000 },
	{ 0xFE70, 0x04B0, 0x0000 },
	{ 0x0190, 0x04B0, 0x0000 },
};

const z64_bgcheck_polygon_t shadowTemplePolyTable[] = {
	{
		0x0000,
		{ 0x0000, 0x0001, 0x0002 },
		0x0000, 0x0000, 0x8001, 0x0000,
	},
	{
		0x0000,
		{ 0x0000, 0x0002, 0x0003 },
		0x0000, 0x0000, 0x8001, 0x0000,
	},
};

const z64_bgcheck_polygon_info_t shadowTemplePolyInfoTable[] = {
	{ 0x00000000, 0x000007C0 },
};

const z64_bgcheck_data_info_t shadowTemplePolyIdData = {
	{ 0xFE70, 0x0000, 0x0000 },
	{ 0x0190, 0x04B0, 0x0000 },
	0x0004,
	&shadowTempleVertexTable,
	0x0002,
	&shadowTemplePolyTable,
	&shadowTemplePolyInfoTable,
	NULL,
	0x0000,
	NULL,
};

/**
 * TYPE:				OFFSET:
 * This Collision		0x00000118
 * Vertex table			0x06000100
 * Poly table			0x060000E0
 * PolyInf table		0x060000D8
 * CamData table		0x060000D0
 * WaterInf table		0x00000000
 * File size			0x0000D6B0
 */
