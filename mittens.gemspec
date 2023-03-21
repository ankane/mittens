require_relative "lib/mittens/version"

Gem::Specification.new do |spec|
  spec.name          = "mittens"
  spec.version       = Mittens::VERSION
  spec.summary       = "Stemming for Ruby, powered by Snowball"
  spec.homepage      = "https://github.com/ankane/mittens"
  spec.license       = "BSD-3-Clause"

  spec.author        = "Andrew Kane"
  spec.email         = "andrew@ankane.org"

  spec.files = Dir.chdir(__dir__) do
    `git ls-files -z --recurse-submodules`.split("\x0").reject do |f|
      (f == __FILE__) || f.match(%r{\A(?:(?:bin|test|spec|features)/|\.(?:git|travis|circleci)|appveyor)})
    end
  end
  spec.require_path  = "lib"
  spec.extensions    = ["ext/mittens/extconf.rb"]

  spec.required_ruby_version = ">= 3"
end
