from google.appengine.ext import db

class Device(db.Model):
    name = db.StringProperty(required=True)
    description = db.TextProperty()
    timezone = db.StringProperty(required=True)
    location = db.GeoPtProperty()

    def __repr__(self):
        return "<%s %s (%r) at 0x%x>" % (
            self.__class__.__name__, self.key(),
            self.name, id(self))

class Sample(db.Model):
    device = db.ReferenceProperty(required=True)
    ts = db.DateTimeProperty(required=True)
    value = db.FloatProperty(required=True)



