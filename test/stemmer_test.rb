require_relative "test_helper"

class TestStemmer < Minitest::Test
  def test_stem
    stemmer = Mittens::Stemmer.new
    assert_equal "tomato", stemmer.stem("tomatoes")
    assert_equal "consign", stemmer.stem("consignment")
  end

  def test_stem_nil
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

  def test_language_unknown
    error = assert_raises(ArgumentError) do
      # use mutable string to catch GC issues faster
      Mittens::Stemmer.new(language: +"hello")
    end
    assert_equal "unknown language: hello", error.message
  end

  def test_language_null_byte
    error = assert_raises(ArgumentError) do
      Mittens::Stemmer.new(language: "english\0")
    end
    assert_equal "string contains null byte", error.message
  end

  def test_languages
    languages = Mittens::Stemmer.languages
    assert_equal 36, languages.size
    assert_includes languages, "english"
  end

  def test_dup
    stemmer = Mittens::Stemmer.new
    assert_equal "tomato", stemmer.dup.stem("tomatoes")
  end

  def test_dup_language_modified
    language = +"english"
    stemmer = Mittens::Stemmer.new(language: language)
    language << "!"
    assert_equal "tomato", stemmer.dup.stem("tomatoes")
  end

  def test_clone
    stemmer = Mittens::Stemmer.new
    assert_equal "tomato", stemmer.clone.stem("tomatoes")
  end
end
