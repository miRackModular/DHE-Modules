name 'FUZZY Z'
hp 9

hue = 240
foreground [hue, 100, 30]
background [hue, 100, 97]

left = hp2mm(1.5)
lc = hp2mm(3.25)
rc = hp2mm(5.75)
right = hp2mm(7.5)

top = 4
dy = 3

y = hp2mm(top + 0 * dy)
port(x: left, y: y, label: 'A')
button(x: lc, y: y, label: '¬')
button(x: rc, y: y, label: '¬')
port(x: right, y: y, label: 'C')

y = hp2mm(top + 1 * dy)
port(x: left, y: y, label: 'B')
button(x: lc, y: y, label: '¬')
button(x: rc, y: y, label: '¬')
port(x: right, y: y, label: 'D')

y = hp2mm(top + 2 * dy)
output_port(x: left, y: y, label: 'AND')
output_port(x: lc, y: y, label: '¬')
output_port(x: rc, y: y, label: '¬')
output_port(x: right, y: y, label: 'AND')

y = hp2mm(top + 3 * dy)
output_port(x: left, y: y, label: 'OR')
output_port(x: lc, y: y, label: '¬')
output_port(x: rc, y: y, label: '¬')
output_port(x: right, y: y, label: 'OR')

y = hp2mm(top + 4 * dy)
output_port(x: left, y: y, label: 'XOR')
output_port(x: lc, y: y, label: '¬')
output_port(x: rc, y: y, label: '¬')
output_port(x: right, y: y, label: 'XOR')

y = hp2mm(top + 5 * dy)
output_port(x: left, y: y, label: 'A &#x22B2; B')
output_port(x: lc, y: y, label: '¬')
output_port(x: rc, y: y, label: '¬')
output_port(x: right, y: y, label: 'C &#x22B2; D')

y = hp2mm(top + 6 * dy)
output_port(x: left, y: y, label: 'A &#x22B3; B')
output_port(x: lc, y: y, label: '¬')
output_port(x: rc, y: y, label: '¬')
output_port(x: right, y: y, label: 'C &#x22B3; D')
