#include <pebble.h>
#include "cgl.h"

Window *my_window;
Layer *window_layer;
Layer *second_layer;
Layer *number_layer;
Layer *radial_layer;
TextLayer *day_layer;

CircGaugeLayer month, hour, minute;

GPoint second = {0,0};

//APP_LOG(APP_LOG_LEVEL_INFO, "end init");

void draw_month(Layer *layer, GContext *ctx) {
	CircGaugeLayer *cgl = &month;
	cgl_draw(cgl, ctx);
}

void draw_hour(Layer *layer, GContext *ctx) {
	CircGaugeLayer *cgl = &hour;
	cgl_draw(cgl, ctx);
}

void draw_minute(Layer *layer, GContext *ctx) {
	CircGaugeLayer *cgl = &minute;
	cgl_draw(cgl, ctx);
}

void draw_radials(Layer *layer, GContext *ctx) {
	GPoint inner, outer;

	graphics_context_set_fill_color(ctx, GColorClear);
	graphics_context_set_stroke_color(ctx, GColorWhite);

	for (uint8_t i=1; i <= 12; i++) {
		int32_t angle = TRIG_MAX_ANGLE * i/12;

		int32_t c_ang = -cos_lookup(angle);
		int32_t s_ang = sin_lookup(angle);

		inner.y = c_ang*(minute.outRadius)/TRIG_MAX_RATIO + month.center.y;
		inner.x = s_ang*(minute.outRadius)/TRIG_MAX_RATIO + month.center.x;
		outer.y = c_ang*(month.outRadius - 8)/TRIG_MAX_RATIO + month.center.y;
		outer.x = s_ang*(month.outRadius - 8)/TRIG_MAX_RATIO + month.center.x;

		graphics_draw_line(ctx, inner, outer);
	}
}

void draw_numbers(Layer *layer, GContext *ctx) {
	GPoint outer;

	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_text_color(ctx, GColorWhite);

	for (uint8_t i=1; i <= 12; i++) {
		int32_t angle = TRIG_MAX_ANGLE * i/12;

		int32_t c_ang = -cos_lookup(angle);
		int32_t s_ang = sin_lookup(angle);

		outer.y = c_ang*(month.outRadius)/TRIG_MAX_RATIO + month.center.y;
		outer.x = s_ang*(month.outRadius)/TRIG_MAX_RATIO + month.center.x;

		char st[3];
		snprintf(st, 3, "%d", i);

		graphics_draw_text(ctx, st, fonts_get_system_font(FONT_KEY_GOTHIC_14),
			GRect(outer.x - 3 * strlen(st), outer.y - 8, 15, 15), GTextOverflowModeWordWrap,
			GTextAlignmentLeft, NULL);
	}
}

void second_layer_draw(Layer *layer, GContext *ctx) {
	graphics_context_set_fill_color(ctx, GColorClear);
	graphics_fill_circle(ctx, second, 4);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, second, 2);
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	static int lastMin = -1, lastHour = -1, lastMon = -1, lastDay = -1;

	int _month = tick_time->tm_mon + 1;
	int _day = tick_time->tm_mday;
	int _hour = tick_time->tm_hour % 12;
	int _minute = tick_time->tm_min;
	int _second = tick_time->tm_sec;

	if (lastMin != _minute) {
		lastMin = _minute;
		minute.value = _minute;

		if (lastHour != _hour) {
			lastHour = _hour;
		}

		hour.value = _hour * 5 + _minute / 12;

		if (lastMon != _month) {
			lastMon = _month;
			month.value = _month * 5;
		}

		if (lastDay != _day) {
			lastDay = _day;

			static char st[3];
			snprintf(st, 3, "%d", _day);
			text_layer_set_text(day_layer, st);
		}
	}

	int32_t second_angle = TRIG_MAX_ANGLE * _second / 60;
	second.y = (-cos_lookup(second_angle) * minute.outRadius / TRIG_MAX_RATIO) + minute.center.y;
	second.x = (sin_lookup(second_angle) * minute.outRadius / TRIG_MAX_RATIO) + minute.center.x;

	layer_mark_dirty(second_layer);
}

void handle_init(void) {
	my_window = window_create();
	window_set_background_color(my_window, GColorBlack);
	window_set_fullscreen(my_window, true);

	month.layer = layer_create(GRect(4, 4, 136, 160));
	cgl_init(&month, draw_month, 0, false, 12);

	number_layer = layer_create(GRect(4, 4, 136, 160));
	layer_set_update_proc(number_layer, draw_numbers);

	hour.layer = layer_create(GRect(22, 22, 100, 124));
	cgl_init(&hour, draw_hour, 1, false, 12);

	minute.layer = layer_create(GRect(40, 40, 64, 88));
	cgl_init(&minute, draw_minute, 1, false, 12);

	second_layer = layer_create(GRect(40, 40, 64, 88));
	layer_set_update_proc(second_layer, second_layer_draw);

	radial_layer = layer_create(GRect(4, 4, 136, 160));
	layer_set_update_proc(radial_layer, draw_radials);

	day_layer = text_layer_create(GRect(57, 65, 30, 30));
	text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);
	text_layer_set_font(day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_background_color(day_layer, GColorBlack);
	text_layer_set_text_color(day_layer, GColorWhite);
	text_layer_set_text(day_layer, "");

	window_layer = window_get_root_layer(my_window);

	layer_add_child(window_layer, month.layer);
	layer_add_child(window_layer, number_layer);
	layer_add_child(window_layer, hour.layer);
	layer_add_child(window_layer, minute.layer);
	layer_add_child(window_layer, second_layer);
	layer_add_child(window_layer, radial_layer);
	layer_add_child(window_layer, text_layer_get_layer(day_layer));

	window_stack_push(my_window, true);

	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

void handle_deinit(void) {
	tick_timer_service_unsubscribe();

	text_layer_destroy(day_layer);
	layer_destroy(radial_layer);
	layer_destroy(second_layer);
	layer_destroy(minute.layer);
	layer_destroy(hour.layer);
	layer_destroy(number_layer);
	layer_destroy(month.layer);

	window_destroy(my_window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
