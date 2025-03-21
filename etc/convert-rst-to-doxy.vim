function! ConvertRstToDoxy()
  silent '<,'>s/.. cpp:function::\(.*\)$/\1;/ge
  silent '<,'>s/\\</</ge
  silent '<,'>s/:cpp:any:`\(.\{-,}\)`/\1/ge
  silent '<,'>s/:\(t\=param \w\+\):\(.*\)\n\s*\/\/\!\s*/\\\1 \2/ge
  silent '<,'>s/:\(t\=param \w\+\):\(.*\)$/\\\1 \2/ge
  silent '<,'>s/:math:`\(.\{-,}\)`/\\f$\1\\f$/ge
  silent '<,'>s/.. code-block:: cpp/\\code/ge
  silent '<,'>s/``/`/ge
  silent '<,'>s/:code://ge
  silent '<,'>s/.. \(\w\+\)::\s*/\\\1 /ge
  silent '<,'>s/:\(\w\+\):/\\\1 /ge
  silent '<,'>s/\*\*Example\*\*/\\par Example/ge
  silent '<,'>s/\/\/\!\s\+/\/\/\! /ge
  silent '<,'>s/this function is `noexcept` and is guaranteed never to throw.\=/\\noexcept/ge
endfunction

map! <F1> <ESC>:call ConvertRstToDoxy()<CR>i
map <F1> :call ConvertRstToDoxy()<CR>
