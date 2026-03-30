# GitHub Gist from James Cracknell (jcracknell)
# https://gist.github.com/jcracknell/52cf4e0f8c4518a853784638db8a258d

def urldecode:
  def unhex:
    if 48 <= . and . <= 57 then . - 48 elif 65 <= . and . <= 70 then . - 55 else . - 87 end;

  def bytes:
    def loop($i):
      if $i >= length then empty else 16 * (.[$i+1] | unhex) + (.[$i+2] | unhex), loop($i+3) end;
    [loop(0)];

  def codepoints:
    def loop($i):
      if $i >= length then empty
      elif .[$i] >= 240 then (.[$i+3]-128) + 64*(.[$i+2]-128) + 4096*(.[$i+1]-128) + 262144*(.[$i]-240), loop($i+4)
      elif .[$i] >= 224 then (.[$i+2]-128) + 64*(.[$i+1]-128) + 4096*(.[$i]-224), loop($i+3)
      elif .[$i] >= 192 then (.[$i+1]-128) + 64*(.[$i]-192), loop($i+2)
      else .[$i], loop($i+1)
      end;
    [loop(0)];

  # Note that URL-encoding implies percent-encoded UTF-8 octets, so we have to
  # manually reassemble these into codepoints for implode
  gsub("(?<m>(?:%[0-9a-fA-F]{2})+)"; .m | explode | bytes | codepoints | implode);
#/urldecode
