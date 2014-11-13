#include <pebble.h>

typedef struct {
	Layer		*layer;
	void (*drawfunc)(Layer*, GContext*);
	uint16_t	sizeX;
	uint16_t	sizeY;
	GPoint		center;
	uint16_t	outRadius;
	uint16_t	inRadius;
	uint16_t	steps;
	bool		doTicks;
	uint16_t	value;
} CircGaugeLayer;

void cgl_draw_ticks(CircGaugeLayer *cgl, GContext *ctx);

void cgl_draw_marker(CircGaugeLayer *cgl, GContext *ctx);

void cgl_draw(CircGaugeLayer *cgl, GContext *ctx);

void cgl_draw_month(Layer *layer, GContext *ctx);

void cgl_draw_hour(Layer *layer, GContext *ctx);

void cgl_draw_minute(Layer *layer, GContext *ctx);

void cgl_init(CircGaugeLayer *cgl, void (*drawfunc)(Layer*,GContext*), uint16_t thickness, bool doticks, uint16_t steps);
