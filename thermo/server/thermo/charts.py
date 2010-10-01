from datetime import datetime, timedelta

from pygooglechart import XYLineChart

from model import Device, Sample

import logging
logger = logging.getLogger("thermo.charts")

def make_chart(request):
    chart = XYLineChart(640, 400)
    # add the devices here to build the legend incrementally
    chart.devices = []
    return chart

def add_day_data(chart, device, date):
    x, y = fetch_data(device, date)
    if not x:
        return False

    chart.add_data(x)
    chart.add_data(y)

    chart.devices.append(device)
    chart.set_legend([d.name for d in chart.devices])
    return True

def fetch_data(device, date):
    ss = Sample.all()
    ss.filter("device =", device)
    ss.filter("ts >=", date)
    ss.filter("ts <", date + timedelta(days=1))
    ss.order("ts")

    x = []; y = []
    mn = datetime(date.year, date.month, date.day)

    # subsample or the request will be too large
    last = -1
    step = 5
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


