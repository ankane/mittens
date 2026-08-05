require "bundler/setup"
Bundler.require(:default)
require "minitest/autorun"

class Minitest::Test
  def setup
    if stress?
      GC.auto_compact = true
      GC.stress = true
    end
  end

  def teardown
    GC.stress = false if stress?
  end

  def stress?
    ENV["STRESS"]
  end
end
