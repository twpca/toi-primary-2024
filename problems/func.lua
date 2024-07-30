function strcmp(A, B)
  A = tostring(A)
  B = tostring(B)
  A = string.gsub(A, "%s+", "")
  B = string.gsub(B, "%s+", "")

  if A == B then
    tex.write("0")
  elseif A < B then
    tex.write("-1")
  else
    tex.write("1")
  end
end

function substr(s, i, j)
  tex.write(string.sub(s, i, j))
end
