import urllib
from datetime import datetime, timedelta

from google.appengine.ext import db
from google.appengine.ext import webapp
from google.appengine.ext.webapp import template
from google.appengine.ext.webapp.util import run_wsgi_app

from model import Device, Sample

import logging
logger = logging.getLogger("thermo.webapp")

class PostSample(webapp.RequestHandler):
    def get(self):
        devices = Device.all()
        msg = self.request.get('msg')
        self.response.out.write(template.render(
            'templates/thermo/add_sample.tmpl',
            {'devices': devices, 'msg': msg}))

    def post(self):
        device = Device.get(self.request.get('device'))

        if self.request.get('data').strip():
            self._post_data(device)
        else:
            self._post_sample(device)

    def _post_sample(self, device):
        ts = self._parse_ts(self.request.get('ts'))
        value = self._parse_value(self.request.get('value'))

        s = Sample(device=device, ts=ts, value=value)
        s.put()
        self.redirect("/sample/%s/" % s.key())

    def _post_data(self, device):
        data = []
        for line in self.request.get('data').splitlines():
            date, time, value = line.split()
            ts = self._parse_ts(date + " " + time)
            value = self._parse_value(value)
            data.append((ts, value))

        for ts, value in data:
            s = Sample(device=device, ts=ts, value=value)
            s.put()

        self.redirect(
            "/sample/new/?msg=%d+samples+added." % len(data))

    def _parse_ts(self, s):
        return datetime.strptime(s, "%Y-%m-%d %H:%M:%S")

    def _parse_value(self, s):
        return float(s)

class NewDevice(webapp.RequestHandler):
    def get(self):
        data = self._request_data()
        self.response.out.write(template.render(
            'templates/thermo/new_device.tmpl',
            {'data': data}))

    def post(self):
        data = self._request_data()
        try:
            device = Device(**data)
            device.put()
        except Exception, e:
            self.response.out.write(template.render(
                'templates/thermo/new_device.tmpl',
                {'data': data, 'msg': "error: %s" % e}))
        else:
            self.redirect("/")



    def _request_data(self):
        rv = {}
        for k in ('name', 'timezone', 'description'):
            rv[k] = self.request.get(k)

        return rv

import charts

class ChartPage(webapp.RequestHandler):
    def get(self, date, device_keys):

        try:
            device_keys = urllib.unquote(device_keys)
            devices = map(Device.get, device_keys.split(","))
        except Exception, e:
            logger.warn("can't fetch device: %s", e)
            return self.error(404)

        try:
            date = datetime.strptime(date, "%Y-%m-%d").date()
        except Exception, e:
            logger.warn("bad date: %s: %s", date, e)
            return self.error(404)

        any = False
        chart = charts.make_chart(self.request)
        for device in devices:
            any = charts.add_day_data(chart, device, date) or any

        charts.finish_chart(chart)

        self.response.out.write(template.render(
            'templates/thermo/chart.tmpl',
            {'devices': devices, 'chart': any and chart, 'date': date}))

class MainPage(webapp.RequestHandler):
    def get(self):
        devices = Device.all()
        date = datetime.now().date()
        self.response.out.write(template.render(
            'templates/thermo/main.tmpl',
            {'devices': devices, 'date': date}))


class ViewSample(webapp.RequestHandler):
    def get(self, key):
        s = Sample.get(key)
        self.response.out.write(template.render(
            'templates/thermo/sample.tmpl',
            {'sample': s}))


app = webapp.WSGIApplication([
    ('/', MainPage),
    ('/sample/new/', PostSample),
    ('/sample/([^/]+)/', ViewSample),
    ('/chart/([^/]+)/([^/]+)/', ChartPage),
    ('/device/new/', NewDevice),
    ],
    debug=True)

def main():
    run_wsgi_app(app)

if __name__ == '__main__':
    main()

