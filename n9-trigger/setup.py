from distutils.core import setup
import os, sys, glob

def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

setup(name="n9-trigger",
      scripts=['n9-trigger'],
      version='0.1.0',
      maintainer="Jonathan Villemaire-Krajden",
      maintainer_email="email@example.com",
      description="A PySide example",
      long_description=read('n9-trigger.longdesc'),
      data_files=[('share/applications',['n9-trigger.desktop']),
                  ('share/icons/hicolor/64x64/apps', ['n9-trigger.png']),
                  ('share/n9-trigger/qml', glob.glob('qml/*.qml')), ],)
