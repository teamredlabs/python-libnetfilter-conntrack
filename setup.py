"""The setup.py script."""

from distutils.core import setup, Extension

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
      ext_modules=[Extension(
          name="libnetfilterconntrack",
          sources=["libnetfilterconntrack.c"],
          libraries=["netfilter_conntrack", "nfnetlink"]
      )])
