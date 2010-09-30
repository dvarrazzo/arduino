from datetime import datetime, timedelta

from google.appengine.ext import db
from google.appengine.ext import webapp
from google.appengine.ext.webapp import template
from google.appengine.ext.webapp.util import run_wsgi_app

from model import Device, Sample

class PostSample(webapp.RequestHandler):
    def get(self):
        devices = Device.all()
        self.response.out.write(template.render(
            'templates/thermo/add_sample.tmpl',
            {'devices': devices}))

    def post(self):
        device = Device.get(self.request.get('device'))
        ts = datetime.strptime(self.request.get('ts'), "%Y-%m-%d %H:%M:%S")
        value = float(self.request.get('value'))

        s = Sample(device=device, ts=ts, value=value)
        s.put()
        self.redirect("/sample/%s/" % s.key())

class MainPage(webapp.RequestHandler):
    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        samples = db.GqlQuery("""
            SELECT * FROM Sample WHERE ts > :1
            ORDER BY ts""",
            datetime.now() - timedelta(hours=24))

        for s in samples:
           self.response.out.write("%s %s\n" % (s.ts, s.value))

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
    ],
    debug=True)

def main():
    run_wsgi_app(app)

if __name__ == '__main__':
    main()

