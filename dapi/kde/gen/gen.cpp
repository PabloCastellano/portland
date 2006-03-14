#include <qfile.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <stdlib.h>

/*

FUNCTION <name>
 ARG <name>
  TYPE <type> - string, string[], int, ...
  OUT         - is used in reply
  RETURN      - this OUT argument is return value of high-level call
 ENDARG
ENDFUNCTION

*/

struct Arg
    {
    Arg() : out( false ), ret( false ) {}
    QString cType( bool out ) const;
    static QString cType( const QString& type, bool out );
    void readCommand( QTextStream& stream ) const;
    void writeCommand( QTextStream& stream ) const;
    QString name;
    QString type;
    bool out;
    bool ret;
    static QValueList< Arg > stripOutArguments( const QValueList< Arg >& );
    static QValueList< Arg > stripNonOutArguments( const QValueList< Arg >& );
    static QValueList< Arg > stripSimpleArguments( const QValueList< Arg >& ); // leaves only string etc.
    static QValueList< Arg > stripReturnArgument( const QValueList< Arg >& );
    };

typedef QValueList< Arg > ArgList;

enum FunctionType { ReadCommand, WriteCommand, ReadReply, WriteReply, HighLevel };

struct Function
    {
    Function() {}
    QString returnType() const;
    QString name;
    ArgList args;
    void generateC( QTextStream& stream, int indent, FunctionType type ) const;
    };

QValueList< Function > functions;

QFile* input_file = NULL;
QTextStream* input_stream = NULL;
static QString last_line;
int last_lineno = 0;

#define check( arg ) my_check( __FILE__, __LINE__, arg )
#define error() my_error( __FILE__, __LINE__ )

void my_error( const char* file, int line )
    {
    fprintf( stderr, "Error: %s: %d\n", file, line );
    fprintf( stderr, "Line %d: %s\n", last_lineno, last_line.utf8().data());
    abort();
    }

void my_check( const char* file, int line, bool arg )
    {
    if( !arg )
        my_error( file, line );
    }

QString Arg::cType( bool out ) const
    {
    return cType( type, out );
    }

QString Arg::cType( const QString& type, bool out )
    {
    if( type.contains( "[]" ))
        return QString( type ).replace( "[]", "arr" );
    if( type == "bool" )
        return "int";
    else if( type == "string" )
        return out ? "char*" : "const char*";
    else
        return type;
    }

ArgList Arg::stripOutArguments( const ArgList& args )
    {
    ArgList new_args;
    for( ArgList::ConstIterator it = args.begin();
         it != args.end();
         ++it )
        {
        const Arg& arg = (*it);
        if( !arg.out )
            new_args.append( arg );
        }
    return new_args;
    }

ArgList Arg::stripNonOutArguments( const ArgList& args )
    {
    ArgList new_args;
    for( ArgList::ConstIterator it = args.begin();
         it != args.end();
         ++it )
        {
        const Arg& arg = (*it);
        if( arg.out )
            new_args.append( arg );
        }
    return new_args;
    }

ArgList Arg::stripSimpleArguments( const ArgList& args )
    {
    ArgList new_args;
    for( ArgList::ConstIterator it = args.begin();
         it != args.end();
         ++it )
        {
        const Arg& arg = (*it);
        if( arg.type == "string"
            || arg.type.contains( "[]" ))
            new_args.append( arg );
        }
    return new_args;
    }

ArgList Arg::stripReturnArgument( const ArgList& args )
    {
    ArgList new_args;
    bool was = false;
    for( ArgList::ConstIterator it = args.begin();
         it != args.end();
         ++it )
        {
        const Arg& arg = (*it);
        if( ! arg.ret )
            new_args.append( arg );
        else
            {
            if( was )
                error();
            was = true;
            }
        }
    return new_args;
    }

void Arg::readCommand( QTextStream& stream ) const
    {
    if( type.endsWith( "[]" ))
        stream << "    *" << name << " = read" << cType( false ) << "( conn );\n";
    else if( type == "string" )
        stream << "    *" << name << " = readString( conn );\n";
    else if( type == "stringlist" )
        stream << "    *" << name << " = readStringList( conn );\n";
    else
        stream << "    readSocket( conn, " << name << ", sizeof( *" << name << " ));\n";
    }

void Arg::writeCommand( QTextStream& stream ) const
    {
    if( type.endsWith( "[]" ))
        stream << "    write" << cType( false ) << "( conn, " << name << " );\n";
    else if( type == "string" )
        stream << "    writeString( conn, " << name << " );\n";
    else if( type == "stringlist" )
        stream << "    writeStringList( conn, " << name << " );\n";
    else
        stream << "    writeSocket( conn, &" << name << ", sizeof( " << name << " ));\n";
    }

QString makeIndent( int indent )
    {
    return indent > 0 ? QString().fill( ' ', indent ) : "";
    }

void Function::generateC( QTextStream& stream, int indent, FunctionType type ) const
    {
    QString line;
    line += makeIndent( indent );
    if( type == HighLevel )
        {
        QString rettype = returnType();
        line += Arg::cType( rettype, true );
        }
    else
        line += type != WriteReply ? "int" : "void";
    line += " dapi_";
    if( type != HighLevel )
        {
        line += type == ReadCommand || type == ReadReply ? "read" : "write";
        line += type == ReadCommand || type == WriteCommand ? "Command" : "Reply";
        }
    line += name;
    line += "( DapiConnection* conn";
    if( type == WriteReply )
        line += ", int seq";
    ArgList args2;
    if( type == HighLevel )
        args2 = Arg::stripReturnArgument( args );
    else if( type == ReadReply || type == WriteReply )
        args2 = Arg::stripNonOutArguments( args );
    else
        args2 = Arg::stripOutArguments( args );
    for( ArgList::ConstIterator it = args2.begin();
         it != args2.end();
         ++it )
        {
        const Arg& arg = (*it);
        line += ",";
        if( line.length() > 80 )
            {
            stream << line << "\n";
            line = makeIndent( indent + 4 );
            }
        else
            line += " ";
        line += arg.cType( type == ReadCommand || type == ReadReply
            || ( type == HighLevel && arg.out ));
        if( type == ReadCommand || type == ReadReply || ( type == HighLevel && arg.out ))
            line += "*";
        line += " " + arg.name;
        }
    line += " )";
    stream << line;
    }

QString Function::returnType() const
    {
    for( ArgList::ConstIterator it = args.begin();
         it != args.end();
         ++it )
        {
        const Arg& arg = (*it);
        if( arg.ret )
            return arg.type;
        }
    return "void";
    }

void openInputFile( const QString& filename )
    {
    check( input_file == NULL );
    input_file = new QFile( filename );
    if( !input_file->open( IO_ReadOnly ))
        error();
    input_stream = new QTextStream( input_file );
    last_lineno = 0;
    }

QString getInputLine()
    {
    while( !input_stream->atEnd())
        {
        QString line = input_stream->readLine().stripWhiteSpace();
        ++last_lineno;
        last_line = line;
        if( line.isEmpty() || line[ 0 ] == '#' )
            continue;
        return line;
        }
    return QString::null;
    }

void closeInputFile()
    {
    delete input_stream;
    delete input_file;
    input_stream = NULL;
    input_file = NULL;
    }

void parseArg( Function& function, const QString& details )
    {
    Arg arg;
    arg.name = details;
    for(;;)
        {
        QString line = getInputLine();
        if( line.isEmpty())
            break;
        if( line.startsWith( "ENDARG" ))
            {
            check( !arg.type.isEmpty());
            function.args.append( arg );
            return;
            }
        else if( line.startsWith( "TYPE" ))
            {
            check( arg.type.isEmpty());
            arg.type = line.mid( strlen( "TYPE" )).stripWhiteSpace();
            }
        else if( line.startsWith( "OUT" ))
            {
            arg.out = true;
            }
        else if( line.startsWith( "RETURN" ))
            {
            arg.out = true;
            arg.ret = true;
            }
        else
            error();
        }
    error();
    }

void parseFunction( const QString& details )
    {
    Function function;
    function.name = details;
    for(;;)
        {
        QString line = getInputLine();
        if( line.isEmpty())
            break;
        if( line.startsWith( "ENDFUNCTION" ))
            {
            functions.append( function );
            return;
            }
        else if( line.startsWith( "ARG" ))
            {
            parseArg( function, line.mid( strlen( "ARG" )).stripWhiteSpace());
            }
        else
            error();
        }
    error();
    }

void parse()
    {
    openInputFile( "gen.txt" );
    for(;;)
        {
        QString line = getInputLine();
        if( line.isEmpty())
            break;
        if( line.startsWith( "FUNCTION" ))
            {
            parseFunction( line.mid( strlen( "FUNCTION" )).stripWhiteSpace());
            }
        else
            error();
        }
    closeInputFile();
    }

void generateSharedCommH()
    {
    QFile file( "comm_generated.h" );
    if( !file.open( IO_WriteOnly ))
        error();
    QTextStream stream( &file );
    stream << "int dapi_readCommand( DapiConnection* conn, int* comm, int* seq );\n";
    for( QValueList< Function >::ConstIterator it = functions.begin();
         it != functions.end();
         ++it )
        {
        const Function& function = *it;
        function.generateC( stream, 0, ReadCommand );
        stream << ";\n";
        function.generateC( stream, 0, WriteCommand );
        stream << ";\n";
        function.generateC( stream, 0, ReadReply );
        stream << ";\n";
        function.generateC( stream, 0, WriteReply );
        stream << ";\n";
        }
    stream << "enum\n    {";
    bool needs_comma = false;
    for( QValueList< Function >::ConstIterator it = functions.begin();
         it != functions.end();
         ++it )
        {
        const Function& function = *it;
        if( needs_comma )
            stream << ",";
        needs_comma = true;
        stream << "\n    DAPI_COMMAND_" << function.name.upper()
               << ",\n    DAPI_REPLY_" << function.name.upper();
        }
    stream << "\n    };\n";
    }

void generateSharedCommCReadFunctions( QTextStream& stream, FunctionType type )
    {
    for( QValueList< Function >::ConstIterator it = functions.begin();
         it != functions.end();
         ++it )
        {
        const Function& function = *it;
        function.generateC( stream, 0, type );
        stream << "\n    {\n";
        ArgList args = type == ReadCommand ? Arg::stripOutArguments( function.args )
            : Arg::stripNonOutArguments( function.args );
        for( ArgList::ConstIterator it = args.begin();
             it != args.end();
             ++it )
            {
            const Arg& arg = (*it);
            arg.readCommand( stream );
            }
        // TODO tady chybi kontrola, ze nebyla chyba pri cteni
        // a udelat to nejak vic genericky
#if 0
        ArgList args_extra = Arg::stripSimpleArguments( args );
        if( !args_extra.isEmpty())
            {
            stream << "    if(";
            bool need_op = false;
            for( ArgList::ConstIterator it = args_extra.begin();
                 it != args_extra.end();
                 ++it )
                {
                const Arg& arg = (*it);
                if( need_op )
                    stream << " ||";
                stream << " *" << arg.name << " == NULL";
                need_op = true;
                }
            stream << " )\n        {\n";
            for( ArgList::ConstIterator it = args_extra.begin();
                 it != args_extra.end();
                 ++it )
                {
                const Arg& arg = (*it);
                stream << "        free( *" << arg.name << " );\n";
                }
            stream << "        return 0;\n        }\n";
            }
#endif
        stream << "    return 1;\n"
               << "    }\n\n";
        }
    }

void generateSharedCommCWriteFunctions( QTextStream& stream, FunctionType type )
    {
    for( QValueList< Function >::ConstIterator it = functions.begin();
         it != functions.end();
         ++it )
        {
        const Function& function = *it;
        function.generateC( stream, 0, type );
        stream << "\n    {\n";
        if( type == WriteCommand )
            {
            stream << "    int seq = getNextSeq( conn );\n"
                   << "    writeCommand( conn, DAPI_COMMAND_" << function.name.upper() << ", seq );\n";
            }
        else
            {
            stream << "    writeCommand( conn, DAPI_REPLY_" << function.name.upper() << ", seq );\n";
            }
        ArgList args2 = type == WriteCommand ? Arg::stripOutArguments( function.args ) : Arg::stripNonOutArguments( function.args );
        for( ArgList::ConstIterator it = args2.begin();
             it != args2.end();
             ++it )
            {
            const Arg& arg = (*it);
            arg.writeCommand( stream );
            }
        // TODO kontrola, ze nebyla chyba pri zapisu?
        if( type == WriteCommand )
            stream << "    return seq;\n";
        stream << "    }\n\n";
        }
    }

void generateSharedCommC()
    {
    QFile file( "comm_generated.c" );
    if( !file.open( IO_WriteOnly ))
        error();
    QTextStream stream( &file );
    generateSharedCommCReadFunctions( stream, ReadCommand );
    generateSharedCommCReadFunctions( stream, ReadReply );
    generateSharedCommCWriteFunctions( stream, WriteCommand );
    generateSharedCommCWriteFunctions( stream, WriteReply );
    }

void generateSharedCallsH()
    {
    QFile file( "calls_generated.h" );
    if( !file.open( IO_WriteOnly ))
        error();
    QTextStream stream( &file );
    for( QValueList< Function >::ConstIterator it = functions.begin();
         it != functions.end();
         ++it )
        {
        const Function& function = *it;
        function.generateC( stream, 0, HighLevel );
        stream << ";\n";
        }
    }

void generateSharedCallsC()
    {
    QFile file( "calls_generated.c" );
    if( !file.open( IO_WriteOnly ))
        error();
    QTextStream stream( &file );
    for( QValueList< Function >::ConstIterator it = functions.begin();
         it != functions.end();
         ++it )
        {
        const Function& function = *it;
        QString rettype = function.returnType();
        function.generateC( stream, 0, HighLevel );
        stream << "\n    {\n"
               << "    int seq;\n"
               << "    if( conn->sync_callback == NULL )\n"
               << "        {\n"
               << "        fprintf( stderr, \"DAPI sync callback not set!\\n\" );\n"
               << "        abort();\n"
               << "        }\n";
        stream << "    " << Arg::cType( rettype, true ) << " ret;\n";
        stream << "    seq = dapi_writeCommand" << function.name << "( conn";
        ArgList args = Arg::stripReturnArgument( function.args );
        ArgList args1 = Arg::stripOutArguments( args );
        for( ArgList::ConstIterator it = args1.begin();
             it != args1.end();
             ++it )
            {
            const Arg& arg = (*it);
            stream << ", " << arg.name;
            }
        stream << " );\n"
               << "    if( seq == 0 )\n"
               << "        return 0;\n";
        stream << "    for(;;)\n"
               << "        {\n"
               << "        int comm, seq2;\n"
               << "        if( !dapi_readCommand( conn, &comm, &seq2 ))\n"
               << "            return 0;\n"
               << "        if( seq2 == seq && comm == DAPI_REPLY_" << function.name.upper() << " )\n"
               << "            break; /* --> */\n"
               << "        conn->sync_callback( conn, comm, seq2 );\n"
               << "        }\n"
               << "    if( !dapi_readReply" << function.name << "( conn";
        ArgList args2 = Arg::stripNonOutArguments( function.args );
        for( ArgList::ConstIterator it = args2.begin();
             it != args2.end();
             ++it )
            {
            const Arg& arg = (*it);
            if( arg.ret )
                stream << ", &ret";
            else
                stream << ", " << arg.name;
            }
        stream << " ))\n"
               << "        return 0;\n";
        if( rettype == "string" )
            {
            // make sure empty return string is really seen as failure
            stream << "    if( ret[ 0 ] == \'\\0\' )\n"
                   << "        {\n"
                   << "        free( ret );\n"
                   << "        ret = NULL;\n"
                   << "        }\n";
            }
        stream << "    return ret;\n"
               << "    }\n\n";
        }
    }

void generateShared()
    {
    generateSharedCommH();
    generateSharedCommC();
    generateSharedCallsH();
    generateSharedCallsC();
    }

void generate()
    {
    generateShared();
    }

int main()
    {
    parse();
    generate();
    return 0;
    }
