require "bundler/gem_tasks"
require "rake/extensiontask"

task :build => :compile

Rake::ExtensionTask.new("noire_structs") do |ext|
  ext.lib_dir = "lib/noire_structs"
end

task :default => [:clobber, :compile, :spec]
