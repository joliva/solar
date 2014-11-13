#include "cgl.h"

void cgl_draw_ticks(CircGaugeLayer *cgl, GContext *ctx) {
	GPoint outer;

	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_fill_color(ctx, GColorWhite);

	for (uint8_t i=1; i <= cgl->steps; i++) {
		int32_t angle = TRIG_MAX_ANGLE * i/cgl->steps;

		int32_t c_ang = -cos_lookup(angle);
		int32_t s_ang = sin_lookup(angle);

		outer.y = c_ang*(cgl->outRadius)/TRIG_MAX_RATIO + cgl->center.y;
		outer.x = s_ang*(cgl->outRadius)/TRIG_MAX_RATIO + cgl->center.x;

		graphics_fill_circle(ctx, outer, 2);
	}
}

void cgl_draw_marker(CircGaugeLayer *cgl, GContext *ctx) {
	GPoint outer;

	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_fill_color(ctx, GColorWhite);

	int32_t angle = (TRIG_MAX_ANGLE * cgl->value)/60;
	int32_t c_ang = -cos_lookup(angle);
	int32_t s_ang = sin_lookup(angle);

	outer.y = c_ang*(cgl->outRadius)/TRIG_MAX_RATIO + cgl->center.y;
	outer.x = s_ang*(cgl->outRadius)/TRIG_MAX_RATIO + cgl->center.x;

	graphics_fill_circle(ctx, outer, 4);
}

void cgl_draw(CircGaugeLayer *cgl, GContext *ctx) {

	// draw outer
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, cgl->center, cgl->outRadius);

	// draw inner
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, cgl->center, cgl->inRadius);

	if (cgl->doTicks == true) {
		cgl_draw_ticks(cgl, ctx);
	}

	cgl_draw_marker(cgl, ctx);
}

void cgl_init(CircGaugeLayer *cgl, void (*drawfunc)(Layer*,GContext*), uint16_t thickness, bool doticks, uint16_t steps) {
	uint16_t sizeX, sizeY;

	cgl->drawfunc = drawfunc;
	cgl->doTicks = doticks;

	GRect bounds = layer_get_bounds(cgl->layer);
	cgl->sizeX = sizeX = bounds.size.w;
	cgl->sizeY = sizeY = bounds.size.h;
	cgl->center = GPoint(sizeX/2, sizeY/2);
	cgl->outRadius = sizeX/2 - 3;
	cgl->inRadius = cgl->outRadius - thickness;
	layer_set_update_proc(cgl->layer, cgl->drawfunc);

	cgl->steps = steps;
	cgl->value = 0;
}
