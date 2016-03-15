# Paragraph designer generation
import sys, time, os.path
styles_438dsg48 = []

def add_style(style):
  styles_438dsg48.append(style)
  return style

class ParagraphOptions:

  def __init__(self, **kw):
    arg_names = ('cmd', 'parent', 'space_above', 'space_below', 'fontsize', 'baseline', 'fontcmd', 'stylecmd', 'boxes', 'leftskip', 'moresetup', 'afterpar', 'env', 'hsize', 'preamble_arg1', 'preamble_arg2', 'preamble_arg3', 'preamble_arg4')
    cmd = kw.get('stylecmd') # 'cmd' or 'stylecmd' are required
    if not cmd:
      cmd = kw['cmd']
    parent = kw.get('parent', None)
    if parent:
      for arg in arg_names:
        if ('cmd' == arg) or ('stylecmd' == arg):
          setattr(self, arg, None)
          continue
        setattr(self, arg, getattr(parent, arg))
    else:
      for arg in arg_names:
        setattr(self, arg, None)
    for (arg, val) in kw.iteritems():
      if not (arg in arg_names):
        raise AttributeError(arg)
      setattr(self, arg, val)

  #
  # Print style as an internal command
  #
  def get_style_string(self):
    s = ''
    #if self.space_above:
    #  s = "\\ParaSpaceAbove{%s}" % self.space_above
    fontcmd = self.fontcmd
    if self.fontsize and self.baseline:
      s = s + "\\fontsize{%s}{%s}" % (self.fontsize, self.baseline)
      if not fontcmd:
        fontcmd = '\\selectfont'
    if fontcmd:
      s = s + fontcmd
    return s

  #
  # Print the style as command
  #
  def print_command(self, h):
    if self.stylecmd:
      s_style = self.get_style_string()
      h.write("\\newcommand{\\%s}{%s}%%\n" % (self.stylecmd, s_style))
    if not self.cmd:
      return
    if self.boxes:
      n_boxes = len(self.boxes)
    else:
      n_boxes = 0
    n_lastarg = n_boxes + 1
    h.write("\\newcommand{\\%s}[%i]{{%%\n" % (self.cmd, n_lastarg))
    self.write_para_setup(h)
    if self.boxes:
      if self.leftskip:
        h.write("\\advance\\pd@leftskip by %s " % self.leftskip)
      self.print_boxes(h)
      h.write("%\n\\the\\everypar ")
    s_after_par = self.get_after_para()
    if self.hsize:
      h.write("\\hsize=%s\\relax\n" % self.hsize)
    preamble_attr = 'preamble_arg' + str(n_lastarg)
    s_preamble = getattr(self, preamble_attr, None)
    if s_preamble is None:
      s_preamble = ''
    h.write("%s#%i\\par}%s}\n" % (s_preamble, n_lastarg, s_after_par))

  def print_env(self, h):
    if not self.env:
      return
    h.write("\\newenvironment{%s}{" % self.env)
    self.write_para_setup(h)
    h.write("\ignorespaces}\n{")
    ap = self.get_after_para()
    if ap:
      h.write("\\par\\global\\def\\pd@after@para{%s}\\aftergroup\\pd@after@para" % ap)
    h.write("}\n")

  def write_para_setup(self, h):
    # "space_above" before "moresetup" because the latter
    # may contain a call to "headbreaker"
    s_style = self.get_style_string()
    h.write(s_style)
    spa = self.space_above or '0pt'
    if  '#natural' != spa:
      h.write("%%\n\\ParaSpaceAbove{%s}" % spa)
    if self.moresetup:
      h.write("%%\n%s" % self.moresetup)
    h.write("%\n\\noindent ")

  def get_after_para(self):
    s_after_par = ''
    if self.afterpar:
      s_after_par = self.afterpar
    if '#natural' != self.space_below:
      spb = self.space_below or '0pt'
      s_after_par = s_after_par + ('\\ParaSpaceBelow{%s}' % spb)
    return s_after_par

  def print_boxes(self, h):
    box_i = 0
    for box in self.boxes:
      box_i = box_i + 1
      (offset, width) = box
      h.write("%%\n\\hbox to 0pt{\\hss\\hbox to %s{#%i\\hss}\\dimen0=%s " % (width, box_i, self.leftskip))
      h.write("%%\n\\advance\\dimen0 by -%s \\advance\\dimen0 by -%s \\hskip\\dimen0}" % (offset, width))

def print_styles(h):
  for style in styles_438dsg48:
    h.write("\n")
    style.print_command(h)
    style.print_env(h)

#
# Command-line
#
def main(codeword):
  h = sys.stdout
  if len(sys.argv) > 1:
    file_name = sys.argv[1]
    h = open(file_name, 'w')
    s_name = os.path.splitext(os.path.basename(file_name))[0]
    s_date = time.strftime('%Y/%m/%d')
    h.write("\\ProvidesPackage{%s}[%s Automatically generated paragraph styles for: %s]\n" % (s_name, s_date, s_name))
    h.write("% RTL hook\n\\let\\pd@leftskip=\\leftskip\n")
  print_styles(h)
  if h != sys.stdout:
    h.close()
