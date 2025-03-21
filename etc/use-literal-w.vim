function! WordTypeToLiteralW()
  silent '<,'>s/\(\d\),\{0,1}\s*\n\{0,1}/\1/ge
  silent '<,'>s/{\s*\(\d\+\)\s*}/\1_w/ge
endfunction

map! <F1> <ESC>:call WordTypeToLiteralW()<CR>i
map <F1> :call WordTypeToLiteralW()<CR>
