#!/bin/bash
html="$(echo $1 | sed -e 's/\.[[:alpha:]]*$/\.html/')"
echo "<html>" > $html
cat $1 | sed -n '/.*:/,$p' | sed '/^$/d' | sed -r -e '1 s/(.*):/<title>\1<\/title>/' | sed -e '1 a <body>' | sed -r -e '3,$ s/(.*)/<p>\1<\/p>/' | sed -e '$ a <\/body>\n<\/html>' >> $html
