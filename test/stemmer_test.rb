require_relative "test_helper"

class TestStemmer < Minitest::Test
  def test_works
    stemmer = Mittens::Stemmer.new
    assert_equal "tomato", stemmer.stem("tomatos")
  end

  def test_nil
    stemmer = Mittens::Stemmer.new
    assert_raises(TypeError) do
      stemmer.stem(nil)
    end
  end

  def test_language
    stemmer = Mittens::Stemmer.new(language: "french")
    assert_equal "tomatos", stemmer.stem("tomatos")
  end

  def test_languages
    assert_includes Mittens::Stemmer.languages, "english"
    assert_equal 29, Mittens::Stemmer.languages.size
  end

  def test_bad_language
    error = assert_raises(ArgumentError) do
      Mittens::Stemmer.new(language: "hello")
    end
    assert_equal "unknown language: hello", error.message
  end
end
