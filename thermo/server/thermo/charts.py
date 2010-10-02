from datetime import datetime, timedelta

from pygooglechart import XYLineChart, Axis

from model import Device, Sample

import logging
logger = logging.getLogger("thermo.charts")

def make_chart(request):
    chart = XYLineChart(640, 400)
    chart.set_colours(['FF0000', '00FF00', '0000FF'])

    # add a few attributes to build the legend incrementally
    chart.devices = []
    chart._ranges = []

    return chart

def finish_chart(chart):
    chart.set_legend([d.name for d in chart.devices])

    if not chart._ranges:
        return

    min_x = min(t[0] for t in chart._ranges)
    max_x = max(t[1] for t in chart._ranges)
    min_y = min(t[2] for t in chart._ranges)
    max_y = max(t[3] for t in chart._ranges)

    chart.set_axis_range(Axis.BOTTOM, min_x, max_x)
    chart.set_axis_range(Axis.LEFT, min_y, max_y)

    # Hack to center the axes names
    chart.set_axis_labels(Axis.BOTTOM, ["", "Hour", ""])
    chart.set_axis_labels(Axis.LEFT, ["", "\xc2\xb0C", ""])


def add_day_data(chart, device, date):
    step = 5
    x, y = fetch_data(device, date, step)
    if not x:
        return False

    if len(x) < 10:
        logger.warn('x: %s', x)
        logger.warn('y: %s', y)
    chart.add_data(x)
    chart.add_data(y)
    chart._ranges.append((
        x[0] / 60., x[-1] / 60.,
        y[0], y[-1]))

    chart.devices.append(device)

    return True

def fetch_data(device, date, step):
    ss = Sample.all()
    ss.filter("device =", device)
    ss.filter("ts >=", date)
    ss.filter("ts <", date + timedelta(days=1))
    ss.order("ts")

    x = []; y = []
    mn = datetime(date.year, date.month, date.day)

    # subsample or the request will be too large
    last = -1
    for i, s in enumerate(ss):
        minute = (s.ts - mn).seconds // 60
        if minute > last:
            logger.info("ts %s: %s", i, s.ts)
            x.append(minute)
            y.append(s.value)
            last = ((minute // step) + 1) * step - 1

    # guard against an empty range
    if x and x[0] < x[-1]:
        return x, y
    else:
        return [], []


