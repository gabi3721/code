#!/usr/bin/env ruby -KU

def print(ifile, ofile, column_no) 
  ext = ifile.split(/\./)[-1]
  type = case ext
         when 'plist'; 'html'
         when 'rb'   ; 'txt'
         when 'py'   ; 'python'
         else
           ext
         end

  #cmd = "enscript -#{column_no}rG --highlight=#{type} --color=1 --line-number -p - #{ifile} | pstopdf -i -o #{ofile}"
  landscape = (column_no == "2") ? 'r' : ''
  cmd = "enscript -#{column_no}#{landscape}G --highlight=#{type} --color=1 --line-number -p - #{ifile} | pstopdf -i -o #{ofile}"
  puts cmd
  `#{cmd}`
end

if __FILE__ == $PROGRAM_NAME
  if ARGV.length < 2
    puts "usage: #{$PROGRAM_NAME} ifile ofile <column_no>"
    exit
  end

  ifile  = ARGV[0]
  ofile  = "#{ARGV[1]}.pdf"
  column_no = ARGV[2]

  unless File.exists?(ifile)
    puts "#{ifile} not exists!"
    exit
  end

  column_no = (ARGV.length == 2) ? "2" : ARGV[2] 
  print(ifile, ofile, column_no)

end
