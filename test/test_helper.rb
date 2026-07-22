require "bundler/setup"
Bundler.require(:default)
require "minitest/autorun"

class Minitest::Test
  def setup
    GC.stress = true if stress?
  end

  def teardown
    GC.stress = false if stress?
  end

  def stress?
    ENV["STRESS"]
  end
end
