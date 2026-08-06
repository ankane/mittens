#include <limits.h>

#include "libstemmer.h"
#include "ruby/ruby.h"

#define GetAlgorithm(language) (NIL_P(language) ? "english" : StringValueCStr(language))
#define GetStemmer(obj, ptr) TypedData_Get_Struct((obj), stemmer_t, &stemmer_data_type, (ptr))

typedef struct stemmer {
    struct sb_stemmer* stemmer;
    VALUE language;
} stemmer_t;

static void stemmer_mark(void* ptr)
{
    stemmer_t* stemmer = (stemmer_t*) ptr;
    rb_gc_mark(stemmer->language);
}

static void stemmer_free(void* ptr)
{
    stemmer_t* stemmer = (stemmer_t*) ptr;
    // safe to pass null pointer, but check anyways
    if (stemmer->stemmer != NULL)
        sb_stemmer_delete(stemmer->stemmer);
    xfree(ptr);
}

const rb_data_type_t stemmer_data_type = {
    .wrap_struct_name = "stemmer",
    .function = {
        .dmark = stemmer_mark,
        .dfree = stemmer_free,
    },
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE stemmer_allocate(VALUE klass)
{
    stemmer_t* stemmer;
    VALUE obj = TypedData_Make_Struct(klass, stemmer_t, &stemmer_data_type, stemmer);
    stemmer->stemmer = NULL;
    stemmer->language = Qnil;
    return obj;
}

static VALUE stemmer_initialize(int argc, VALUE* argv, VALUE self)
{
    stemmer_t* stemmer;
    GetStemmer(self, stemmer);

    VALUE opts;
    rb_scan_args(argc, argv, ":", &opts);

    if (!NIL_P(opts))
        stemmer->language = rb_hash_aref(opts, ID2SYM(rb_intern("language")));

    const char* algorithm = GetAlgorithm(stemmer->language);

    // in case called multiple times
    // TODO raise error
    if (stemmer->stemmer != NULL)
        sb_stemmer_delete(stemmer->stemmer);

    // if adding support for encoding, may want to change encoding returned from stem
    stemmer->stemmer = sb_stemmer_new(algorithm, NULL);
    if (stemmer->stemmer == NULL)
        rb_raise(rb_eArgError, "unknown language: %s", algorithm);

    // must be placed after last use of algorithm
    RB_GC_GUARD(stemmer->language);

    return self;
}

static VALUE stemmer_stem(VALUE self, VALUE value)
{
    stemmer_t* stemmer;
    GetStemmer(self, stemmer);

    if (stemmer->stemmer == NULL)
        rb_raise(rb_eRuntimeError, "stemmer not initialized");

    const char* word = StringValuePtr(value);
    long size = RSTRING_LEN(value);
    if (size > INT_MAX)
        rb_raise(rb_eArgError, "string exceeds max length");

    const sb_symbol* pointer_out = sb_stemmer_stem(stemmer->stemmer, (const sb_symbol*) word, (int) size);
    int length_out = sb_stemmer_length(stemmer->stemmer);

    return rb_utf8_str_new((char*) pointer_out, (long) length_out);
}

static VALUE stemmer_initialize_copy(VALUE self, VALUE orig)
{
    stemmer_t* stemmer;
    GetStemmer(self, stemmer);

    stemmer_t* orig_stemmer;
    GetStemmer(orig, orig_stemmer);

    if (stemmer->stemmer != NULL)
        rb_raise(rb_eRuntimeError, "stemmer already initialized");

    stemmer->stemmer = sb_stemmer_new(GetAlgorithm(orig_stemmer->language), NULL);
    if (stemmer->stemmer == NULL)
        rb_raise(rb_eRuntimeError, "could not copy stemmer");

    stemmer->language = orig_stemmer->language;

    return self;
}

static VALUE stemmer_languages(VALUE klass)
{
    VALUE out = rb_ary_new();

    const char** language = sb_stemmer_list();
    while (*language != NULL)
        rb_ary_push(out, rb_utf8_str_new_cstr(*language++));

    return out;
}

void Init_ext(void)
{
    VALUE mMittens = rb_define_module("Mittens");
    VALUE cStemmer = rb_define_class_under(mMittens, "Stemmer", rb_cObject);
    rb_define_alloc_func(cStemmer, stemmer_allocate);
    rb_define_method(cStemmer, "initialize", stemmer_initialize, -1);
    rb_define_method(cStemmer, "stem", stemmer_stem, 1);
    rb_define_method(cStemmer, "initialize_copy", stemmer_initialize_copy, 1);
    rb_define_singleton_method(cStemmer, "languages", stemmer_languages, 0);
}
