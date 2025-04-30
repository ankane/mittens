#ifdef USE_SYSTEM_LIBSTEMMER
#include <libstemmer.h>
#else
#include "libstemmer.h"
#endif

#include "ruby/ruby.h"

typedef struct stemmer {
    struct sb_stemmer * stemmer;
} stemmer_t;

static void stemmer_free(void *ptr)
{
    stemmer_t *stemmer = (stemmer_t *)ptr;
    // safe to pass null pointer according to docs
    sb_stemmer_delete(stemmer->stemmer);
    xfree(ptr);
}

const rb_data_type_t stemmer_data_type = {
    .wrap_struct_name = "stemmer",
    .function = {
        .dfree = stemmer_free,
    },
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE stemmer_allocate(VALUE klass)
{
    stemmer_t *stemmer;
    VALUE obj = TypedData_Make_Struct(klass, stemmer_t, &stemmer_data_type, stemmer);
    stemmer->stemmer = NULL;
    return obj;
}

static VALUE stemmer_initialize(int argc, VALUE* argv, VALUE self)
{
    VALUE opts;
    rb_scan_args(argc, argv, ":", &opts);

    const char * algorithm = "english";
    if (!NIL_P(opts)) {
        VALUE language = rb_hash_aref(opts, ID2SYM(rb_intern("language")));
        if (!NIL_P(language)) {
            Check_Type(language, T_STRING);
            algorithm = RSTRING_PTR(language);
        }
    }

    stemmer_t *stemmer;
    TypedData_Get_Struct(self, stemmer_t, &stemmer_data_type, stemmer);

    // in case called multiple times
    sb_stemmer_delete(stemmer->stemmer);

    // if adding support for encoding, may want to change encoding returned from stem
    stemmer->stemmer = sb_stemmer_new(algorithm, NULL);
    if (stemmer->stemmer == NULL) {
        rb_raise(rb_eArgError, "unknown language: %s", algorithm);
    }

    return self;
}

static VALUE stemmer_stem(VALUE self, VALUE value)
{
    stemmer_t *stemmer;
    TypedData_Get_Struct(self, stemmer_t, &stemmer_data_type, stemmer);

    Check_Type(value, T_STRING);

    const sb_symbol * word = (const sb_symbol *) RSTRING_PTR(value);
    int size = (int) RSTRING_LEN(value);
    const sb_symbol * pointer_out = sb_stemmer_stem(stemmer->stemmer, word, size);

    return rb_utf8_str_new_cstr((char *) pointer_out);
}

static VALUE stemmer_languages(VALUE klass)
{
    VALUE out = rb_ary_new();

    const char **language = sb_stemmer_list();
    while (*language != NULL) {
        rb_ary_push(out, rb_utf8_str_new_cstr(*language));
        language++;
    }

    return out;
}

void Init_ext(void)
{
    VALUE rb_mMittens = rb_define_module("Mittens");
    VALUE rb_cStemmer = rb_define_class_under(rb_mMittens, "Stemmer", rb_cObject);
    rb_define_alloc_func(rb_cStemmer, stemmer_allocate);
    rb_define_method(rb_cStemmer, "initialize", stemmer_initialize, -1);
    rb_define_method(rb_cStemmer, "stem", stemmer_stem, 1);
    rb_define_singleton_method(rb_cStemmer, "languages", stemmer_languages, 0);
}
