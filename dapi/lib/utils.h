void debug( const char* fmt, ... )
#ifdef __GNUC__
    __attribute (( format( printf, 1, 2 )))
#endif
    ;
