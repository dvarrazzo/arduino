from datetime import datetime, timedelta

from google.appengine.ext import db
from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

class ThermoSample(db.Model):
    ts = db.DateTimeProperty()
    temp = db.FloatProperty()


class PostSample(webapp.RequestHandler):
    def post(self):

        import sys
        import pdb
#        for attr in ('stdin', 'stdout', 'stderr'):
#            setattr(sys, attr, getattr(sys, '__%s__' % attr))
#        pdb.set_trace()

        ts = datetime.strptime(self.request.get('ts'), "%Y-%m-%d %H:%M:%S")
        temp = float(self.request.get('temp'))

        s = ThermoSample(ts=ts, temp=temp)
        s.put()

        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write('ok')

class MainPage(webapp.RequestHandler):
    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        samples = db.GqlQuery("""
            SELECT * FROM ThermoSample WHERE ts > :1
            ORDER BY ts""",
            datetime.now() - timedelta(hours=24))
        
        for s in samples:
           self.response.out.write("%s %s\n" % (s.ts, s.temp))

app = webapp.WSGIApplication([
    ('/', MainPage),
    ('/add', PostSample),
    ],
    debug=True)

def main():
    run_wsgi_app(app)

if __name__ == '__main__':
    main()

