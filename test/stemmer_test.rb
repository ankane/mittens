require_relative "test_helper"

class TestStemmer < Minitest::Test
  def test_works
    stemmer = Mittens::Stemmer.new
    assert_equal "tomato", stemmer.stem("tomatoes")
    assert_equal "consign", stemmer.stem("consignment")
  end

  def test_nil
    stemmer = Mittens::Stemmer.new
    assert_raises(TypeError) do
      stemmer.stem(nil)
    end
  end

  def test_language
    stemmer = Mittens::Stemmer.new(language: "french")
    assert_equal "continu", stemmer.stem("continuait")
    assert_equal "mainten", stemmer.stem("maintenaient")
  end

  def test_languages
    languages = Mittens::Stemmer.languages
    assert_equal 32, languages.size
    assert_includes languages, "english"
  end

  def test_bad_language
    error = assert_raises(ArgumentError) do
      Mittens::Stemmer.new(language: "hello")
    end
    assert_equal "unknown language: hello", error.message
  end
end
