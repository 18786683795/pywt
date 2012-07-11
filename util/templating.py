#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Author: Filip Wasilewski
# Date: $Date$

# Ok, not really a full-featured templating language, but good enough
# to keep the code easier to maintain.
# PS. For internal use only ;)

from __future__ import print_function
import os
import re # sounds fun, doesn't it?

pattern_for = re.compile(r"""(?P<for>
                                ^\s*
                                    (?:/{2,})?  # optional C comment
                                    \s* 
                                    \#{2}       # two hashes
                                    \s*  
                                       
                                    (FOR)
                                        \s+ (?P<variable>[\w$][\d\w$]*) \s+
                                    (IN)
                                        \s+ \(
                                        (?P<values>
                                            (?:
                                                \s* [^,\s]+ , \s*
                                            )+
                                            (?:
                                                [^,\s]+
                                            ){0,1}
                                            \s*
                                        )
                                        \)
                                    \s* : \s*
                             )
                             ^(?P<body>.*?)
                             (?P<endfor>
                                ^
                                    \s*
                                    (?:/{2,})?  # optional C comment
                                    \s*
                                    \#{2}       # two hashes
                                    \s*
                                    (ENDFOR)
                                        \s+ (?P=variable) \s*?\n
                             )
""", re.X | re.M | re.S | re.I)

def expand_template(s):
    """
    Currently it only does a simple repeat-and-replace in a loop:
    
    FOR $variable$ IN (value1, value2, ...):
        ... start block ...
        $variable$
        ... end block ...
    ENDFOR $variable$
    
    The above will repeat the block for every value from the list each time
    substituting the $variable$ with the current value.

        >>> s = \
        ... '''
        ... w = 9
        ... ## FOR $x$ IN (7, w):
        ...   ## FOR $y$ IN ("{", 1):
        ... print $x$, $y$, "$x$_$y$"
        ...   ## ENDFOR $y$
        ... ## ENDFOR $x$'''
        >>> print expand_template(s)
        
        w = 9
        print 7, "{"
        print 7, 1
        print w, "{"
        print w, 1
    """
    while True:
        m = pattern_for.search(s)
        if not m:
            break
        
        new_body = ''
        for value in [v.strip() for v in m.group('values').split(',') if v.strip()]:
            new_body += m.group('body').replace(m.group('variable'), value)
    
        s = s[:m.start()] + new_body + s[m.end():]

    return s

def expand_files(glob_pattern, force_update=False):
    import glob
    from os.path import splitext, exists, getmtime, basename, dirname, join, extsep
    files = glob.glob(os.path.abspath(glob_pattern))

    for name in files:
        directory = dirname(name)
        new_name, ext = splitext(basename(name)) # main extension
        while extsep in new_name:
            # remove .template extension for files like file.template.c
            new_name = splitext(new_name)[0]
        new_name = join(directory, new_name + ext)
        if not exists(new_name) or force_update or getmtime(new_name) < getmtime(name):
            print("expanding template '{0}' -> '{1}'".format(name, new_name))
            content = expand_template(open(name, "rb").read().decode("utf-8"))
            new_file = open(new_name, "wb")
            new_file.write(content.encode("utf-8"))
            new_file.close()
