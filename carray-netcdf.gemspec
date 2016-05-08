
Gem::Specification::new do |s|
  version = "1.0.0"

  files = Dir.glob("**/*") - [ 
                               Dir.glob("carray-*.gem"), 
                             ].flatten

  s.platform    = Gem::Platform::RUBY
  s.name        = "carray-netcdf"
  s.summary     = "Extension for manipulating NetCDF3 file with CArray"
  s.description = <<-HERE
    Extension for manipulating NetCDF3 file with CArray
  HERE
  s.version     = version
  s.author      = "Hiroki Motoyoshi"
  s.email       = ""
  s.homepage    = 'https://github.com/himotoyoshi/carray-netcdf'
  s.files       = files
  s.extensions  = [ "extconf.rb" ]
  s.has_rdoc    = false
  s.required_ruby_version = ">= 1.8.1"
end
