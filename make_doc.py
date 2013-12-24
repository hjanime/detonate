import argparse, io, subprocess
import xml.etree.cElementTree as cet

# Parse arguments.
p = argparse.ArgumentParser()
p.add_argument("--template", required=True, help="The template file. Input.")
p.add_argument("--html", required=True, help="The html file. Output.")
p.add_argument("--text", required=True, help="The text file. Output.")
p.add_argument("--cxx", required=True, help="The C++ header file. Output.")
args = p.parse_args()

# Read template.
tree = cet.parse(args.template)
root = tree.getroot()

# Fill in the head element.
head = root.find("head")

e = cet.Comment("This file is autogenerated. Edit the template instead.")
e.tail = "\n"
head.append(e)

e = cet.Element("script")
e.set("type", "text/x-mathjax-config")
e.text = """
MathJax.Hub.Config({
  tex2jax: {inlineMath: [['$','$']]}
});
"""
e.tail = "\n"
head.append(e)

e = cet.Element("script")
e.set("type", "text/javascript")
e.set("src", "http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_SVG")
e.tail = "\n"
head.append(e)

e = cet.Element("style")
e.set("type", "text/css")
e.text = """
body { max-width: 50em; }
body, td { font-family: sans-serif; }
.arg { margin: 1.2em 0em 1.2em 1.2ex; font: fixed; }
.adef { margin: 0.6em 0em 0.6em 4.8ex; }
h1, h2 { color: #990000; }
"""
e.tail = "\n"
head.append(e)

# Write the DOM to a HTML string.
bio = io.BytesIO()
tree.write(bio, method="html", encoding="utf-8")
html = bio.getvalue().decode("utf-8")

# Replace quotation marks.
html = html.replace("``", "&ldquo;")
html = html.replace("''$", "NF_PRIME_PRIME_DOLLAR_PLACEHOLDER")
html = html.replace("''", "&rdquo;")
html = html.replace("NF_PRIME_PRIME_DOLLAR_PLACEHOLDER", "''$")

# Write the HTML string to the output file.
with open(args.html, "w") as fo:
  fo.write(html)

# Make the text version of the output.
with open(args.text, "w") as fo:
  subprocess.check_call([
    "elinks",
    "-dump", args.html,
    "-dump-width", "75"
    ], stdout=fo)

# Make the C header version of the output.
with open(args.cxx, "w") as fo:
  print("// This file is autogenerated. Edit the template instead.", file=fo)
  print("std::string get_help_string() { return", file=fo)
  for l in open(args.text):
    l = l.strip("\n")
    l = l.replace("\\", "\\\\")
    l = l.replace("\"", "\\\"")
    print('"{}\\n"'.format(l), file=fo)
  print("; }", file=fo)
