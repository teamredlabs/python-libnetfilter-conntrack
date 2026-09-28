"""The setup.py script."""

import os

from setuptools import setup, Extension
from setuptools.command.build_py import build_py


class libnetfilter_build_py(build_py):

    def run(self):
        build_py.run(self)
        dest = os.path.join(
            self.build_lib,
            'libnetfilterconntrack-stubs',
            '__init__.pyi',
        )
        self.mkpath(os.path.dirname(dest))
        self.copy_file('libnetfilterconntrack.pyi', dest)


setup(name="python-libnetfilter-conntrack",
      version='0.0.1',
      description='Python wrapper for libnetfilter_conntrack',
      author='John Lawrence M. Penafiel',
      author_email='jonh@teamredlabs.com',
      license='BSD-2-Clause',
      url='https://github.com/teamredlabs/python-libnetfilter-conntrack',
      classifiers=['Development Status :: 4 - Beta',
                   'Environment :: Plugins',
                   'Intended Audience :: Developers',
                   'Intended Audience :: Information Technology',
                   'Intended Audience :: System Administrators',
                   'License :: OSI Approved :: BSD License',
                   'Operating System :: POSIX :: Linux',
                   'Programming Language :: C',
                   'Programming Language :: Python :: 2.7',
                   'Topic :: Communications',
                   'Topic :: Internet :: Log Analysis',
                   'Topic :: System :: Networking :: Monitoring'],
      keywords='libnetfilter libnetfilterconntrack netfilter conntrack',
      ext_modules=[Extension(name="libnetfilterconntrack",
                             sources=["libnetfilterconntrack.c"],
                             libraries=["netfilter_conntrack", "nfnetlink"])],
      cmdclass={'build_py': libnetfilter_build_py},
      packages=['libnetfilterconntrack-stubs'],
      zip_safe=False)
