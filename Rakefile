require 'pathname'

INCLUDE_FOLEDERS = Dir.glob('**/include')
COMPILER = "g++"
LINKER = "g++"
COMPILE_FLAGS = ["-std=c++14",INCLUDE_FOLEDERS.map{|i| "-I #{i}"}].flatten.join(" ")
LINK_FLAGS = ""
EXE_NAME = "IslandMUD-Server"

dirs = []
object_files = []
source_files = []

source_files = Dir.glob('**/*.c{,pp}').map do |itm| Pathname.new itm end
source_files.each do |file|
    # Get the objects and dependencies
    data = `#{COMPILER} #{COMPILE_FLAGS} -MM #{file}`
    object_file = data[0...data.index(":")]
    data = data[(data.index(":") + 1)..-1].split.map{|i| i.strip}.reject{|i| i == "\\"}

    o_file = file.dirname / 'obj' / file.basename.sub_ext('.o')
    object_files << o_file
    dirs << o_file.dirname
    file o_file => [o_file.dirname, data].flatten do
        sh "#{COMPILER} #{COMPILE_FLAGS} -c -o #{o_file} #{file}"
    end
end

dirs = dirs.uniq

dirs.each do |dir|
    directory dir
end

task :all => object_files
task :clean do
    object_files.each{|i| i.delete if i.exist?}
end

directory "bin"
file "bin/#{EXE_NAME}" => ["bin", :all] do
    sh "#{LINKER} -o bin/#{EXE_NAME} #{object_files.join(" ")}"
end

task :default => "bin/#{EXE_NAME}"
