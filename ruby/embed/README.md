루비 embedding
==============

* 루비 1.8과 1.9를 나누어서 생각한다.

* 디자인
  3 가지로 나누어 생각이 가능하다.

  1. scriptable catalog 
  2. rule
  3. app. automation

  * scriptable catalog
    vim의 구조를 살펴보면 된다.

    print "Hello"                          # displays a message
    VIM.command(cmd)                       # execute an ex command
    num  = VIM::Window.count               # gets the number of windows
    w    = VIM::Window[n]                  # gets window "n"
    cw   = VIM::Window.current             # gets the current window
    num  = VIM::Buffer.count               # gets the number of buffers
    b    = VIM::Buffer[n]                  # gets buffer "n"
    cb   = VIM::Buffer.current             # gets the current buffer
    w.height = lines                       # sets the window height
    w.cursor = [row, col]                  # sets the window cursor position
    pos  = w.cursor                        # gets an array [row, col]
    name = b.name                          # gets the buffer file name
    line = b[n]                            # gets a line from the buffer
    num  = b.count                         # gets the number of lines
    b[n] = str                             # sets a line in the buffer
    b.delete(n)                            # deletes a line
    b.append(n, str)                       # appends a line after n
    line = VIM::Buffer.current.line        # gets the current line
    num  = VIM::Buffer.current.line_number # gets the current line number
    VIM::Buffer.current.line = "test"      # sets the current line number


* 구현
  1. C API를 충분히 숙지한다.

  2. 가장 기본적인 정보는 if_ruby.c에 있다. 충분히 분석한다

  3. C++ extension을 만들 때에는 rice를 이용한다.
     rice는 intrusive하다. 완성된 예제가 rice/if_ruby안에 있다.

* rice 학습
  1. source code중에서 test분석

  2. http://rubyforge.net/rice의 main.html

  3. http://rubyforge.net/rice의 mailing list


* 기타
  두 언어를 섞어서 쓰는 것은 어려운 점이 많다. 특히 서로 이질적인 
  (그래서 섞기도 하지만) 두 언어를 합칠 때 정말 생각지 못한 많은 
  문제들이 나타난다.

  배열 IO그리고 
  host language에서 정의한 객체를 루비로 넘길 때의 문제

  swig는 이 본질적인 문제를 잘 다루고 있다.
