true

g++ -c foo.c

: : :

cat < /etc/passwd | tr a-z A-Z | sort -u || echo sort failed!

a b	<	c > d

cat < /etc/passwd | tr a-z A-Z | sort -u > out || echo sort failed!

a&&b||
 c &&
  d | e && f|

g<h

# However simple this is, we should not omit.
a<b>c|d<e>f|g<h>i
    d<e>f
  |
    g<h>i
   &&
  h>i<j
   ||
   g<t
