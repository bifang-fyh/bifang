
#line 1 "src/uri.rl"
#include "uri.h"

namespace bifang
{


#line 10 "src/uri.cpp"
static const int uri_parser_start = 231;
static const int uri_parser_first_final = 231;
static const int uri_parser_error = 0;

static const int uri_parser_en_main = 231;


#line 115 "src/uri.rl"



// static
Uri::ptr Uri::create(const std::string& uristr)
{
    Uri::ptr uri(new Uri);
    std::string t_uristr;
    t_uristr.resize(uristr.size());
    std::transform(uristr.begin(), uristr.end(), t_uristr.begin(), ::tolower);
    int cs = 0;
    const char* mark = 0;
    
#line 32 "src/uri.cpp"
	{
	cs = uri_parser_start;
	}

#line 128 "src/uri.rl"
    const char *p = t_uristr.c_str();
    const char *pe = p + t_uristr.size();
    const char* eof = pe;
    
#line 42 "src/uri.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 231:
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr234;
		case 37: goto st3;
		case 47: goto tr236;
		case 58: goto st0;
		case 60: goto st0;
		case 62: goto st0;
		case 63: goto tr237;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 91 ) {
		if ( (*p) > 32 ) {
			if ( 65 <= (*p) && (*p) <= 90 )
				goto tr238;
		} else if ( (*p) >= 0 )
			goto st0;
	} else if ( (*p) > 94 ) {
		if ( (*p) > 122 ) {
			if ( 123 <= (*p) && (*p) <= 125 )
				goto st0;
		} else if ( (*p) >= 97 )
			goto tr238;
	} else
		goto st0;
	goto st232;
st232:
	if ( ++p == pe )
		goto _test_eof232;
case 232:
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr234;
		case 37: goto st3;
		case 47: goto tr239;
		case 58: goto st0;
		case 60: goto st0;
		case 62: goto st0;
		case 63: goto tr237;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 91 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 94 ) {
		if ( 123 <= (*p) && (*p) <= 125 )
			goto st0;
	} else
		goto st0;
	goto st232;
st0:
cs = 0;
	goto _out;
tr234:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st233;
tr243:
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st233;
tr247:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 48 "src/uri.rl"
	{
        uri->setQuery(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st233;
tr249:
#line 48 "src/uri.rl"
	{
        uri->setQuery(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st233;
tr254:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st233;
tr265:
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st233;
tr269:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 40 "src/uri.rl"
	{
        if (p != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st233;
tr273:
#line 40 "src/uri.rl"
	{
        if (p != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st233;
st233:
	if ( ++p == pe )
		goto _test_eof233;
case 233:
#line 202 "src/uri.cpp"
	switch( (*p) ) {
		case 37: goto tr241;
		case 60: goto st0;
		case 62: goto st0;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 34 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 35 ) {
		if ( (*p) > 94 ) {
			if ( 123 <= (*p) && (*p) <= 125 )
				goto st0;
		} else if ( (*p) >= 91 )
			goto st0;
	} else
		goto st0;
	goto tr240;
tr240:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st234;
st234:
	if ( ++p == pe )
		goto _test_eof234;
case 234:
#line 230 "src/uri.cpp"
	switch( (*p) ) {
		case 37: goto st1;
		case 60: goto st0;
		case 62: goto st0;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 34 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 35 ) {
		if ( (*p) > 94 ) {
			if ( 123 <= (*p) && (*p) <= 125 )
				goto st0;
		} else if ( (*p) >= 91 )
			goto st0;
	} else
		goto st0;
	goto st234;
tr241:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 258 "src/uri.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st2;
	} else
		goto st2;
	goto st0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st234;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st234;
	} else
		goto st234;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st4;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st4;
	} else
		goto st4;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st232;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st232;
	} else
		goto st232;
	goto st0;
tr239:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st235;
tr256:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st235;
tr266:
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st235;
tr270:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 40 "src/uri.rl"
	{
        if (p != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
	goto st235;
tr274:
#line 40 "src/uri.rl"
	{
        if (p != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st235;
st235:
	if ( ++p == pe )
		goto _test_eof235;
case 235:
#line 357 "src/uri.cpp"
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr243;
		case 37: goto st5;
		case 60: goto st0;
		case 62: goto st0;
		case 63: goto tr245;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 91 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 94 ) {
		if ( 123 <= (*p) && (*p) <= 125 )
			goto st0;
	} else
		goto st0;
	goto st235;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st6;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st6;
	} else
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st235;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st235;
	} else
		goto st235;
	goto st0;
tr237:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st236;
tr245:
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st236;
tr262:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st236;
tr268:
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st236;
tr272:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 40 "src/uri.rl"
	{
        if (p != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st236;
tr276:
#line 40 "src/uri.rl"
	{
        if (p != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st236;
st236:
	if ( ++p == pe )
		goto _test_eof236;
case 236:
#line 485 "src/uri.cpp"
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr247;
		case 37: goto tr248;
		case 60: goto st0;
		case 62: goto st0;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 91 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 94 ) {
		if ( 123 <= (*p) && (*p) <= 125 )
			goto st0;
	} else
		goto st0;
	goto tr246;
tr246:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st237;
st237:
	if ( ++p == pe )
		goto _test_eof237;
case 237:
#line 512 "src/uri.cpp"
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr249;
		case 37: goto st7;
		case 60: goto st0;
		case 62: goto st0;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 91 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 94 ) {
		if ( 123 <= (*p) && (*p) <= 125 )
			goto st0;
	} else
		goto st0;
	goto st237;
tr248:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 539 "src/uri.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st8;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st8;
	} else
		goto st8;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st237;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st237;
	} else
		goto st237;
	goto st0;
tr236:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st238;
st238:
	if ( ++p == pe )
		goto _test_eof238;
case 238:
#line 570 "src/uri.cpp"
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr243;
		case 37: goto st5;
		case 47: goto st239;
		case 60: goto st0;
		case 62: goto st0;
		case 63: goto tr245;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 91 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 94 ) {
		if ( 123 <= (*p) && (*p) <= 125 )
			goto st0;
	} else
		goto st0;
	goto st235;
st239:
	if ( ++p == pe )
		goto _test_eof239;
case 239:
	switch( (*p) ) {
		case 2: goto tr252;
		case 33: goto tr253;
		case 35: goto tr254;
		case 37: goto tr255;
		case 47: goto tr256;
		case 48: goto tr257;
		case 49: goto tr258;
		case 50: goto tr259;
		case 58: goto tr261;
		case 59: goto tr253;
		case 61: goto tr253;
		case 63: goto tr262;
		case 64: goto tr263;
		case 91: goto tr264;
		case 95: goto tr253;
		case 126: goto tr253;
	}
	if ( (*p) < 51 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto tr253;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto tr253;
		} else if ( (*p) >= 65 )
			goto tr253;
	} else
		goto tr260;
	goto st0;
tr252:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 633 "src/uri.cpp"
	if ( 48 <= (*p) && (*p) <= 52 )
		goto st10;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st11;
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) == 46 )
		goto st12;
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 2: goto st13;
		case 48: goto st15;
		case 49: goto st27;
		case 50: goto st29;
	}
	if ( 51 <= (*p) && (*p) <= 57 )
		goto st28;
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( 48 <= (*p) && (*p) <= 52 )
		goto st14;
	goto st0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st15;
	goto st0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) == 46 )
		goto st16;
	goto st0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	switch( (*p) ) {
		case 2: goto st17;
		case 48: goto st19;
		case 49: goto st23;
		case 50: goto st25;
	}
	if ( 51 <= (*p) && (*p) <= 57 )
		goto st24;
	goto st0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( 48 <= (*p) && (*p) <= 52 )
		goto st18;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st19;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) == 46 )
		goto st20;
	goto st0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	switch( (*p) ) {
		case 2: goto st21;
		case 48: goto st240;
		case 49: goto st243;
		case 50: goto st245;
	}
	if ( 51 <= (*p) && (*p) <= 57 )
		goto st244;
	goto st0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	if ( 48 <= (*p) && (*p) <= 52 )
		goto st22;
	goto st0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st240;
	goto st0;
st240:
	if ( ++p == pe )
		goto _test_eof240;
case 240:
	switch( (*p) ) {
		case 35: goto tr265;
		case 47: goto tr266;
		case 58: goto tr267;
		case 63: goto tr268;
	}
	goto st0;
tr287:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st241;
tr267:
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st241;
st241:
	if ( ++p == pe )
		goto _test_eof241;
case 241:
#line 777 "src/uri.cpp"
	switch( (*p) ) {
		case 35: goto tr269;
		case 47: goto tr270;
		case 63: goto tr272;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr271;
	goto st0;
tr271:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st242;
st242:
	if ( ++p == pe )
		goto _test_eof242;
case 242:
#line 794 "src/uri.cpp"
	switch( (*p) ) {
		case 35: goto tr273;
		case 47: goto tr274;
		case 63: goto tr276;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st242;
	goto st0;
st243:
	if ( ++p == pe )
		goto _test_eof243;
case 243:
	switch( (*p) ) {
		case 35: goto tr265;
		case 47: goto tr266;
		case 58: goto tr267;
		case 63: goto tr268;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st244;
	goto st0;
st244:
	if ( ++p == pe )
		goto _test_eof244;
case 244:
	switch( (*p) ) {
		case 35: goto tr265;
		case 47: goto tr266;
		case 58: goto tr267;
		case 63: goto tr268;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st240;
	goto st0;
st245:
	if ( ++p == pe )
		goto _test_eof245;
case 245:
	switch( (*p) ) {
		case 35: goto tr265;
		case 47: goto tr266;
		case 53: goto st246;
		case 58: goto tr267;
		case 63: goto tr268;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st240;
	goto st0;
st246:
	if ( ++p == pe )
		goto _test_eof246;
case 246:
	switch( (*p) ) {
		case 35: goto tr265;
		case 47: goto tr266;
		case 58: goto tr267;
		case 63: goto tr268;
	}
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st240;
	goto st0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) == 46 )
		goto st20;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st24;
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) == 46 )
		goto st20;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st19;
	goto st0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	switch( (*p) ) {
		case 46: goto st20;
		case 53: goto st26;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st19;
	goto st0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 46 )
		goto st20;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st19;
	goto st0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) == 46 )
		goto st16;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st28;
	goto st0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( (*p) == 46 )
		goto st16;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st15;
	goto st0;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	switch( (*p) ) {
		case 46: goto st16;
		case 53: goto st30;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st15;
	goto st0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) == 46 )
		goto st16;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st15;
	goto st0;
tr253:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st247;
st247:
	if ( ++p == pe )
		goto _test_eof247;
case 247:
#line 940 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 47: goto tr266;
		case 58: goto tr279;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st247;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st247;
	} else
		goto st247;
	goto st0;
tr255:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 970 "src/uri.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st32;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st32;
	} else
		goto st32;
	goto st0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st247;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st247;
	} else
		goto st247;
	goto st0;
tr261:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st248;
tr279:
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st248;
st248:
	if ( ++p == pe )
		goto _test_eof248;
case 248:
#line 1013 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st33;
		case 35: goto tr269;
		case 37: goto st34;
		case 47: goto tr270;
		case 61: goto st33;
		case 63: goto tr272;
		case 64: goto tr38;
		case 95: goto st33;
		case 126: goto st33;
	}
	if ( (*p) < 58 ) {
		if ( (*p) > 46 ) {
			if ( 48 <= (*p) && (*p) <= 57 )
				goto tr280;
		} else if ( (*p) >= 36 )
			goto st33;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto st0;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	switch( (*p) ) {
		case 33: goto st33;
		case 37: goto st34;
		case 61: goto st33;
		case 64: goto tr38;
		case 95: goto st33;
		case 126: goto st33;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto st33;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto st0;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st35;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st35;
	} else
		goto st35;
	goto st0;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st33;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st33;
	} else
		goto st33;
	goto st0;
tr38:
#line 25 "src/uri.rl"
	{
        uri->setUserinfo(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st249;
tr263:
#line 17 "src/uri.rl"
	{ mark = p; }
#line 25 "src/uri.rl"
	{
        uri->setUserinfo(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st249;
st249:
	if ( ++p == pe )
		goto _test_eof249;
case 249:
#line 1110 "src/uri.cpp"
	switch( (*p) ) {
		case 2: goto tr252;
		case 33: goto tr281;
		case 35: goto tr254;
		case 37: goto tr282;
		case 47: goto tr256;
		case 48: goto tr283;
		case 49: goto tr284;
		case 50: goto tr285;
		case 58: goto tr287;
		case 59: goto tr281;
		case 61: goto tr281;
		case 63: goto tr262;
		case 91: goto tr264;
		case 95: goto tr281;
		case 126: goto tr281;
	}
	if ( (*p) < 51 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto tr281;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto tr281;
		} else if ( (*p) >= 65 )
			goto tr281;
	} else
		goto tr286;
	goto st0;
tr281:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st250;
st250:
	if ( ++p == pe )
		goto _test_eof250;
case 250:
#line 1148 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 47: goto tr266;
		case 58: goto tr267;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st250;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st250;
	} else
		goto st250;
	goto st0;
tr282:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 1177 "src/uri.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st37;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st37;
	} else
		goto st37;
	goto st0;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st250;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st250;
	} else
		goto st250;
	goto st0;
tr283:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st251;
st251:
	if ( ++p == pe )
		goto _test_eof251;
case 251:
#line 1208 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st252;
		case 47: goto tr266;
		case 58: goto tr267;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st250;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st250;
	} else
		goto st250;
	goto st0;
st252:
	if ( ++p == pe )
		goto _test_eof252;
case 252:
	switch( (*p) ) {
		case 2: goto st13;
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 47: goto tr266;
		case 48: goto st253;
		case 49: goto st261;
		case 50: goto st263;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 51 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st262;
	goto st0;
st253:
	if ( ++p == pe )
		goto _test_eof253;
case 253:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st254;
		case 47: goto tr266;
		case 58: goto tr267;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st250;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st250;
	} else
		goto st250;
	goto st0;
st254:
	if ( ++p == pe )
		goto _test_eof254;
case 254:
	switch( (*p) ) {
		case 2: goto st17;
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 47: goto tr266;
		case 48: goto st255;
		case 49: goto st257;
		case 50: goto st259;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 51 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st258;
	goto st0;
st255:
	if ( ++p == pe )
		goto _test_eof255;
case 255:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st256;
		case 47: goto tr266;
		case 58: goto tr267;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st250;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st250;
	} else
		goto st250;
	goto st0;
st256:
	if ( ++p == pe )
		goto _test_eof256;
case 256:
	switch( (*p) ) {
		case 2: goto st21;
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 47: goto tr266;
		case 58: goto tr267;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st250;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st250;
	} else
		goto st250;
	goto st0;
st257:
	if ( ++p == pe )
		goto _test_eof257;
case 257:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st256;
		case 47: goto tr266;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st258;
	goto st0;
st258:
	if ( ++p == pe )
		goto _test_eof258;
case 258:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st256;
		case 47: goto tr266;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st255;
	goto st0;
st259:
	if ( ++p == pe )
		goto _test_eof259;
case 259:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st256;
		case 47: goto tr266;
		case 53: goto st260;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st255;
	goto st0;
st260:
	if ( ++p == pe )
		goto _test_eof260;
case 260:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st256;
		case 47: goto tr266;
		case 58: goto tr267;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 54 ) {
		if ( (*p) > 45 ) {
			if ( 48 <= (*p) && (*p) <= 53 )
				goto st255;
		} else if ( (*p) >= 36 )
			goto st250;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st250;
	goto st0;
st261:
	if ( ++p == pe )
		goto _test_eof261;
case 261:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st254;
		case 47: goto tr266;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st262;
	goto st0;
st262:
	if ( ++p == pe )
		goto _test_eof262;
case 262:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st254;
		case 47: goto tr266;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st253;
	goto st0;
st263:
	if ( ++p == pe )
		goto _test_eof263;
case 263:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st254;
		case 47: goto tr266;
		case 53: goto st264;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st253;
	goto st0;
st264:
	if ( ++p == pe )
		goto _test_eof264;
case 264:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st254;
		case 47: goto tr266;
		case 58: goto tr267;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 54 ) {
		if ( (*p) > 45 ) {
			if ( 48 <= (*p) && (*p) <= 53 )
				goto st253;
		} else if ( (*p) >= 36 )
			goto st250;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st250;
	goto st0;
tr284:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st265;
st265:
	if ( ++p == pe )
		goto _test_eof265;
case 265:
#line 1615 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st252;
		case 47: goto tr266;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st266;
	goto st0;
tr286:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st266;
st266:
	if ( ++p == pe )
		goto _test_eof266;
case 266:
#line 1649 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st252;
		case 47: goto tr266;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st251;
	goto st0;
tr285:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st267;
st267:
	if ( ++p == pe )
		goto _test_eof267;
case 267:
#line 1683 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st252;
		case 47: goto tr266;
		case 53: goto st268;
		case 58: goto tr267;
		case 59: goto st250;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st250;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st251;
	goto st0;
st268:
	if ( ++p == pe )
		goto _test_eof268;
case 268:
	switch( (*p) ) {
		case 33: goto st250;
		case 35: goto tr265;
		case 37: goto st36;
		case 46: goto st252;
		case 47: goto tr266;
		case 58: goto tr267;
		case 61: goto st250;
		case 63: goto tr268;
		case 95: goto st250;
		case 126: goto st250;
	}
	if ( (*p) < 54 ) {
		if ( (*p) > 45 ) {
			if ( 48 <= (*p) && (*p) <= 53 )
				goto st251;
		} else if ( (*p) >= 36 )
			goto st250;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st250;
		} else if ( (*p) >= 65 )
			goto st250;
	} else
		goto st250;
	goto st0;
tr264:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 1749 "src/uri.cpp"
	switch( (*p) ) {
		case 58: goto st146;
		case 118: goto st225;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st39;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st39;
	} else
		goto st39;
	goto st0;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	if ( (*p) == 58 )
		goto st43;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st40;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st40;
	} else
		goto st40;
	goto st0;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	if ( (*p) == 58 )
		goto st43;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st41;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st41;
	} else
		goto st41;
	goto st0;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
	if ( (*p) == 58 )
		goto st43;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st42;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st42;
	} else
		goto st42;
	goto st0;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	if ( (*p) == 58 )
		goto st43;
	goto st0;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	if ( (*p) == 58 )
		goto st141;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st44;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st44;
	} else
		goto st44;
	goto st0;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
	if ( (*p) == 58 )
		goto st48;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st45;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st45;
	} else
		goto st45;
	goto st0;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	if ( (*p) == 58 )
		goto st48;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st46;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st46;
	} else
		goto st46;
	goto st0;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	if ( (*p) == 58 )
		goto st48;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st47;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st47;
	} else
		goto st47;
	goto st0;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	if ( (*p) == 58 )
		goto st48;
	goto st0;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	if ( (*p) == 58 )
		goto st136;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st49;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st49;
	} else
		goto st49;
	goto st0;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	if ( (*p) == 58 )
		goto st53;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st50;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st50;
	} else
		goto st50;
	goto st0;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	if ( (*p) == 58 )
		goto st53;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st51;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st51;
	} else
		goto st51;
	goto st0;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	if ( (*p) == 58 )
		goto st53;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st52;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st52;
	} else
		goto st52;
	goto st0;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	if ( (*p) == 58 )
		goto st53;
	goto st0;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	if ( (*p) == 58 )
		goto st131;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st54;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st54;
	} else
		goto st54;
	goto st0;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	if ( (*p) == 58 )
		goto st58;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st55;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st55;
	} else
		goto st55;
	goto st0;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( (*p) == 58 )
		goto st58;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st56;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st56;
	} else
		goto st56;
	goto st0;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	if ( (*p) == 58 )
		goto st58;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st57;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st57;
	} else
		goto st57;
	goto st0;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( (*p) == 58 )
		goto st58;
	goto st0;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	if ( (*p) == 58 )
		goto st126;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st59;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st59;
	} else
		goto st59;
	goto st0;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	if ( (*p) == 58 )
		goto st63;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st60;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st60;
	} else
		goto st60;
	goto st0;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	if ( (*p) == 58 )
		goto st63;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st61;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st61;
	} else
		goto st61;
	goto st0;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	if ( (*p) == 58 )
		goto st63;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st62;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st62;
	} else
		goto st62;
	goto st0;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	if ( (*p) == 58 )
		goto st63;
	goto st0;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	if ( (*p) == 58 )
		goto st113;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st64;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st64;
	} else
		goto st64;
	goto st0;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	if ( (*p) == 58 )
		goto st68;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st65;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st65;
	} else
		goto st65;
	goto st0;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	if ( (*p) == 58 )
		goto st68;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st66;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st66;
	} else
		goto st66;
	goto st0;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	if ( (*p) == 58 )
		goto st68;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st67;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st67;
	} else
		goto st67;
	goto st0;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	if ( (*p) == 58 )
		goto st68;
	goto st0;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	switch( (*p) ) {
		case 2: goto st69;
		case 48: goto st96;
		case 49: goto st104;
		case 50: goto st107;
		case 58: goto st111;
	}
	if ( (*p) < 65 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st110;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st112;
	} else
		goto st112;
	goto st0;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	if ( 48 <= (*p) && (*p) <= 52 )
		goto st70;
	goto st0;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st71;
	goto st0;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	if ( (*p) == 46 )
		goto st72;
	goto st0;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	switch( (*p) ) {
		case 2: goto st73;
		case 48: goto st75;
		case 49: goto st92;
		case 50: goto st94;
	}
	if ( 51 <= (*p) && (*p) <= 57 )
		goto st93;
	goto st0;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( 48 <= (*p) && (*p) <= 52 )
		goto st74;
	goto st0;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st75;
	goto st0;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( (*p) == 46 )
		goto st76;
	goto st0;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	switch( (*p) ) {
		case 2: goto st77;
		case 48: goto st79;
		case 49: goto st88;
		case 50: goto st90;
	}
	if ( 51 <= (*p) && (*p) <= 57 )
		goto st89;
	goto st0;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	if ( 48 <= (*p) && (*p) <= 52 )
		goto st78;
	goto st0;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st79;
	goto st0;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	if ( (*p) == 46 )
		goto st80;
	goto st0;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 2: goto st81;
		case 48: goto st83;
		case 49: goto st84;
		case 50: goto st86;
	}
	if ( 51 <= (*p) && (*p) <= 57 )
		goto st85;
	goto st0;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( 48 <= (*p) && (*p) <= 52 )
		goto st82;
	goto st0;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st83;
	goto st0;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( (*p) == 93 )
		goto st240;
	goto st0;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	if ( (*p) == 93 )
		goto st240;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st85;
	goto st0;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	if ( (*p) == 93 )
		goto st240;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st83;
	goto st0;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	switch( (*p) ) {
		case 53: goto st87;
		case 93: goto st240;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st83;
	goto st0;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	if ( (*p) == 93 )
		goto st240;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st83;
	goto st0;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( (*p) == 46 )
		goto st80;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st89;
	goto st0;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	if ( (*p) == 46 )
		goto st80;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st79;
	goto st0;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
	switch( (*p) ) {
		case 46: goto st80;
		case 53: goto st91;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st79;
	goto st0;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	if ( (*p) == 46 )
		goto st80;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st79;
	goto st0;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( (*p) == 46 )
		goto st76;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st93;
	goto st0;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	if ( (*p) == 46 )
		goto st76;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st75;
	goto st0;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
	switch( (*p) ) {
		case 46: goto st76;
		case 53: goto st95;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st75;
	goto st0;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	if ( (*p) == 46 )
		goto st76;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st75;
	goto st0;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st100;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st97;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st97;
	} else
		goto st97;
	goto st0;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
	if ( (*p) == 58 )
		goto st100;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st98;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st98;
	} else
		goto st98;
	goto st0;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
	if ( (*p) == 58 )
		goto st100;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st99;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st99;
	} else
		goto st99;
	goto st0;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	if ( (*p) == 58 )
		goto st100;
	goto st0;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	if ( (*p) == 58 )
		goto st83;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st101;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st101;
	} else
		goto st101;
	goto st0;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( (*p) == 93 )
		goto st240;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st102;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st102;
	} else
		goto st102;
	goto st0;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	if ( (*p) == 93 )
		goto st240;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st103;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st103;
	} else
		goto st103;
	goto st0;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	if ( (*p) == 93 )
		goto st240;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st83;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st83;
	} else
		goto st83;
	goto st0;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st100;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st105;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st97;
	} else
		goto st97;
	goto st0;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st100;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st106;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st98;
	} else
		goto st98;
	goto st0;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st100;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st99;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st99;
	} else
		goto st99;
	goto st0;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
	switch( (*p) ) {
		case 46: goto st72;
		case 53: goto st109;
		case 58: goto st100;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st108;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st97;
	} else
		goto st97;
	goto st0;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st100;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st98;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st98;
	} else
		goto st98;
	goto st0;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st100;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st106;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st98;
		} else if ( (*p) >= 65 )
			goto st98;
	} else
		goto st98;
	goto st0;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st100;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st108;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st97;
	} else
		goto st97;
	goto st0;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st101;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st101;
	} else
		goto st101;
	goto st0;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	if ( (*p) == 58 )
		goto st100;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st97;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st97;
	} else
		goto st97;
	goto st0;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	switch( (*p) ) {
		case 2: goto st69;
		case 48: goto st114;
		case 49: goto st118;
		case 50: goto st121;
	}
	if ( (*p) < 65 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st124;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st125;
	} else
		goto st125;
	goto st0;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st115;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st115;
	} else
		goto st115;
	goto st0;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	if ( (*p) == 58 )
		goto st111;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st116;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st116;
	} else
		goto st116;
	goto st0;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	if ( (*p) == 58 )
		goto st111;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st117;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st117;
	} else
		goto st117;
	goto st0;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	if ( (*p) == 58 )
		goto st111;
	goto st0;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st119;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st115;
	} else
		goto st115;
	goto st0;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st120;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st116;
	} else
		goto st116;
	goto st0;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st117;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st117;
	} else
		goto st117;
	goto st0;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
	switch( (*p) ) {
		case 46: goto st72;
		case 53: goto st123;
		case 58: goto st111;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st122;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st115;
	} else
		goto st115;
	goto st0;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st116;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st116;
	} else
		goto st116;
	goto st0;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st120;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st116;
		} else if ( (*p) >= 65 )
			goto st116;
	} else
		goto st116;
	goto st0;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st122;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st115;
	} else
		goto st115;
	goto st0;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	if ( (*p) == 58 )
		goto st111;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st115;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st115;
	} else
		goto st115;
	goto st0;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st127;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st127;
	} else
		goto st127;
	goto st0;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( (*p) == 58 )
		goto st113;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st128;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st128;
	} else
		goto st128;
	goto st0;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	if ( (*p) == 58 )
		goto st113;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st129;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st129;
	} else
		goto st129;
	goto st0;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	if ( (*p) == 58 )
		goto st113;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st130;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st130;
	} else
		goto st130;
	goto st0;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	if ( (*p) == 58 )
		goto st113;
	goto st0;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st132;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st132;
	} else
		goto st132;
	goto st0;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	if ( (*p) == 58 )
		goto st126;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st133;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st133;
	} else
		goto st133;
	goto st0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	if ( (*p) == 58 )
		goto st126;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st134;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st134;
	} else
		goto st134;
	goto st0;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	if ( (*p) == 58 )
		goto st126;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st135;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st135;
	} else
		goto st135;
	goto st0;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
	if ( (*p) == 58 )
		goto st126;
	goto st0;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st137;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st137;
	} else
		goto st137;
	goto st0;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	if ( (*p) == 58 )
		goto st131;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st138;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st138;
	} else
		goto st138;
	goto st0;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	if ( (*p) == 58 )
		goto st131;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st139;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st139;
	} else
		goto st139;
	goto st0;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	if ( (*p) == 58 )
		goto st131;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st140;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st140;
	} else
		goto st140;
	goto st0;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( (*p) == 58 )
		goto st131;
	goto st0;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st142;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st142;
	} else
		goto st142;
	goto st0;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( (*p) == 58 )
		goto st136;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st143;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st143;
	} else
		goto st143;
	goto st0;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	if ( (*p) == 58 )
		goto st136;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st144;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st144;
	} else
		goto st144;
	goto st0;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	if ( (*p) == 58 )
		goto st136;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st145;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st145;
	} else
		goto st145;
	goto st0;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	if ( (*p) == 58 )
		goto st136;
	goto st0;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
	if ( (*p) == 58 )
		goto st147;
	goto st0;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
	switch( (*p) ) {
		case 2: goto st69;
		case 48: goto st148;
		case 49: goto st217;
		case 50: goto st220;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st223;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st224;
	} else
		goto st224;
	goto st0;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st149;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st149;
	} else
		goto st149;
	goto st0;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
	switch( (*p) ) {
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st150;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st150;
	} else
		goto st150;
	goto st0;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
	switch( (*p) ) {
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st151;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st151;
	} else
		goto st151;
	goto st0;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
	switch( (*p) ) {
		case 58: goto st152;
		case 93: goto st240;
	}
	goto st0;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
	switch( (*p) ) {
		case 2: goto st69;
		case 48: goto st153;
		case 49: goto st209;
		case 50: goto st212;
	}
	if ( (*p) < 65 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st215;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st216;
	} else
		goto st216;
	goto st0;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st154;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st154;
	} else
		goto st154;
	goto st0;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
	switch( (*p) ) {
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st155;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st155;
	} else
		goto st155;
	goto st0;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
	switch( (*p) ) {
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st156;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st156;
	} else
		goto st156;
	goto st0;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
	switch( (*p) ) {
		case 58: goto st157;
		case 93: goto st240;
	}
	goto st0;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
	switch( (*p) ) {
		case 2: goto st69;
		case 48: goto st158;
		case 49: goto st201;
		case 50: goto st204;
	}
	if ( (*p) < 65 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st207;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st208;
	} else
		goto st208;
	goto st0;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st159;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st159;
	} else
		goto st159;
	goto st0;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
	switch( (*p) ) {
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st160;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st160;
	} else
		goto st160;
	goto st0;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
	switch( (*p) ) {
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st161;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st161;
	} else
		goto st161;
	goto st0;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
	switch( (*p) ) {
		case 58: goto st162;
		case 93: goto st240;
	}
	goto st0;
st162:
	if ( ++p == pe )
		goto _test_eof162;
case 162:
	switch( (*p) ) {
		case 2: goto st69;
		case 48: goto st163;
		case 49: goto st193;
		case 50: goto st196;
	}
	if ( (*p) < 65 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st199;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st200;
	} else
		goto st200;
	goto st0;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st164;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st164;
	} else
		goto st164;
	goto st0;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
	switch( (*p) ) {
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st165;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st165;
	} else
		goto st165;
	goto st0;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
	switch( (*p) ) {
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st166;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st166;
	} else
		goto st166;
	goto st0;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
	switch( (*p) ) {
		case 58: goto st167;
		case 93: goto st240;
	}
	goto st0;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
	switch( (*p) ) {
		case 2: goto st69;
		case 48: goto st168;
		case 49: goto st185;
		case 50: goto st188;
	}
	if ( (*p) < 65 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st191;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st192;
	} else
		goto st192;
	goto st0;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st169;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st169;
	} else
		goto st169;
	goto st0;
st169:
	if ( ++p == pe )
		goto _test_eof169;
case 169:
	switch( (*p) ) {
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st170;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st170;
	} else
		goto st170;
	goto st0;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
	switch( (*p) ) {
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st171;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st171;
	} else
		goto st171;
	goto st0;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
	switch( (*p) ) {
		case 58: goto st172;
		case 93: goto st240;
	}
	goto st0;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
	switch( (*p) ) {
		case 2: goto st69;
		case 48: goto st173;
		case 49: goto st177;
		case 50: goto st180;
	}
	if ( (*p) < 65 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st183;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st184;
	} else
		goto st184;
	goto st0;
st173:
	if ( ++p == pe )
		goto _test_eof173;
case 173:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st174;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st174;
	} else
		goto st174;
	goto st0;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
	switch( (*p) ) {
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st175;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st175;
	} else
		goto st175;
	goto st0;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
	switch( (*p) ) {
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st176;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st176;
	} else
		goto st176;
	goto st0;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
	switch( (*p) ) {
		case 58: goto st111;
		case 93: goto st240;
	}
	goto st0;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st178;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st174;
	} else
		goto st174;
	goto st0;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st179;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st175;
	} else
		goto st175;
	goto st0;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st176;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st176;
	} else
		goto st176;
	goto st0;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
	switch( (*p) ) {
		case 46: goto st72;
		case 53: goto st182;
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st181;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st174;
	} else
		goto st174;
	goto st0;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st175;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st175;
	} else
		goto st175;
	goto st0;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st179;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st175;
		} else if ( (*p) >= 65 )
			goto st175;
	} else
		goto st175;
	goto st0;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st181;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st174;
	} else
		goto st174;
	goto st0;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
	switch( (*p) ) {
		case 58: goto st111;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st174;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st174;
	} else
		goto st174;
	goto st0;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st186;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st169;
	} else
		goto st169;
	goto st0;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st187;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st170;
	} else
		goto st170;
	goto st0;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st171;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st171;
	} else
		goto st171;
	goto st0;
st188:
	if ( ++p == pe )
		goto _test_eof188;
case 188:
	switch( (*p) ) {
		case 46: goto st72;
		case 53: goto st190;
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st189;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st169;
	} else
		goto st169;
	goto st0;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st170;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st170;
	} else
		goto st170;
	goto st0;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st187;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st170;
		} else if ( (*p) >= 65 )
			goto st170;
	} else
		goto st170;
	goto st0;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st189;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st169;
	} else
		goto st169;
	goto st0;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
	switch( (*p) ) {
		case 58: goto st172;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st169;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st169;
	} else
		goto st169;
	goto st0;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st194;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st164;
	} else
		goto st164;
	goto st0;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st195;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st165;
	} else
		goto st165;
	goto st0;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st166;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st166;
	} else
		goto st166;
	goto st0;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
	switch( (*p) ) {
		case 46: goto st72;
		case 53: goto st198;
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st197;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st164;
	} else
		goto st164;
	goto st0;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st165;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st165;
	} else
		goto st165;
	goto st0;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st195;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st165;
		} else if ( (*p) >= 65 )
			goto st165;
	} else
		goto st165;
	goto st0;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st197;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st164;
	} else
		goto st164;
	goto st0;
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
	switch( (*p) ) {
		case 58: goto st167;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st164;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st164;
	} else
		goto st164;
	goto st0;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st202;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st159;
	} else
		goto st159;
	goto st0;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st203;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st160;
	} else
		goto st160;
	goto st0;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st161;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st161;
	} else
		goto st161;
	goto st0;
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
	switch( (*p) ) {
		case 46: goto st72;
		case 53: goto st206;
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st205;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st159;
	} else
		goto st159;
	goto st0;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st160;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st160;
	} else
		goto st160;
	goto st0;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st203;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st160;
		} else if ( (*p) >= 65 )
			goto st160;
	} else
		goto st160;
	goto st0;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st205;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st159;
	} else
		goto st159;
	goto st0;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
	switch( (*p) ) {
		case 58: goto st162;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st159;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st159;
	} else
		goto st159;
	goto st0;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st210;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st154;
	} else
		goto st154;
	goto st0;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st211;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st155;
	} else
		goto st155;
	goto st0;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st156;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st156;
	} else
		goto st156;
	goto st0;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
	switch( (*p) ) {
		case 46: goto st72;
		case 53: goto st214;
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st213;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st154;
	} else
		goto st154;
	goto st0;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st155;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st155;
	} else
		goto st155;
	goto st0;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st211;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st155;
		} else if ( (*p) >= 65 )
			goto st155;
	} else
		goto st155;
	goto st0;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st213;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st154;
	} else
		goto st154;
	goto st0;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
	switch( (*p) ) {
		case 58: goto st157;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st154;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st154;
	} else
		goto st154;
	goto st0;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st218;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st149;
	} else
		goto st149;
	goto st0;
st218:
	if ( ++p == pe )
		goto _test_eof218;
case 218:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st219;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st150;
	} else
		goto st150;
	goto st0;
st219:
	if ( ++p == pe )
		goto _test_eof219;
case 219:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st151;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st151;
	} else
		goto st151;
	goto st0;
st220:
	if ( ++p == pe )
		goto _test_eof220;
case 220:
	switch( (*p) ) {
		case 46: goto st72;
		case 53: goto st222;
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st221;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st149;
	} else
		goto st149;
	goto st0;
st221:
	if ( ++p == pe )
		goto _test_eof221;
case 221:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st150;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st150;
	} else
		goto st150;
	goto st0;
st222:
	if ( ++p == pe )
		goto _test_eof222;
case 222:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st219;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st150;
		} else if ( (*p) >= 65 )
			goto st150;
	} else
		goto st150;
	goto st0;
st223:
	if ( ++p == pe )
		goto _test_eof223;
case 223:
	switch( (*p) ) {
		case 46: goto st72;
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st221;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st149;
	} else
		goto st149;
	goto st0;
st224:
	if ( ++p == pe )
		goto _test_eof224;
case 224:
	switch( (*p) ) {
		case 58: goto st152;
		case 93: goto st240;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st149;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st149;
	} else
		goto st149;
	goto st0;
st225:
	if ( ++p == pe )
		goto _test_eof225;
case 225:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st226;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st226;
	} else
		goto st226;
	goto st0;
st226:
	if ( ++p == pe )
		goto _test_eof226;
case 226:
	if ( (*p) == 46 )
		goto st227;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st226;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st226;
	} else
		goto st226;
	goto st0;
st227:
	if ( ++p == pe )
		goto _test_eof227;
case 227:
	switch( (*p) ) {
		case 33: goto st228;
		case 36: goto st228;
		case 61: goto st228;
		case 95: goto st228;
		case 126: goto st228;
	}
	if ( (*p) < 48 ) {
		if ( 38 <= (*p) && (*p) <= 46 )
			goto st228;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st228;
		} else if ( (*p) >= 65 )
			goto st228;
	} else
		goto st228;
	goto st0;
st228:
	if ( ++p == pe )
		goto _test_eof228;
case 228:
	switch( (*p) ) {
		case 33: goto st228;
		case 36: goto st228;
		case 61: goto st228;
		case 93: goto st240;
		case 95: goto st228;
		case 126: goto st228;
	}
	if ( (*p) < 48 ) {
		if ( 38 <= (*p) && (*p) <= 46 )
			goto st228;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st228;
		} else if ( (*p) >= 65 )
			goto st228;
	} else
		goto st228;
	goto st0;
tr280:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st269;
st269:
	if ( ++p == pe )
		goto _test_eof269;
case 269:
#line 4596 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st33;
		case 35: goto tr273;
		case 37: goto st34;
		case 47: goto tr274;
		case 61: goto st33;
		case 63: goto tr276;
		case 64: goto tr38;
		case 95: goto st33;
		case 126: goto st33;
	}
	if ( (*p) < 58 ) {
		if ( (*p) > 46 ) {
			if ( 48 <= (*p) && (*p) <= 57 )
				goto st269;
		} else if ( (*p) >= 36 )
			goto st33;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto st0;
tr257:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st270;
st270:
	if ( ++p == pe )
		goto _test_eof270;
case 270:
#line 4631 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st271;
		case 47: goto tr266;
		case 58: goto tr279;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st247;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st247;
	} else
		goto st247;
	goto st0;
st271:
	if ( ++p == pe )
		goto _test_eof271;
case 271:
	switch( (*p) ) {
		case 2: goto st13;
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 47: goto tr266;
		case 48: goto st272;
		case 49: goto st280;
		case 50: goto st282;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 51 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st281;
	goto st0;
st272:
	if ( ++p == pe )
		goto _test_eof272;
case 272:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st273;
		case 47: goto tr266;
		case 58: goto tr279;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st247;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st247;
	} else
		goto st247;
	goto st0;
st273:
	if ( ++p == pe )
		goto _test_eof273;
case 273:
	switch( (*p) ) {
		case 2: goto st17;
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 47: goto tr266;
		case 48: goto st274;
		case 49: goto st276;
		case 50: goto st278;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 51 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st277;
	goto st0;
st274:
	if ( ++p == pe )
		goto _test_eof274;
case 274:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st275;
		case 47: goto tr266;
		case 58: goto tr279;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st247;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st247;
	} else
		goto st247;
	goto st0;
st275:
	if ( ++p == pe )
		goto _test_eof275;
case 275:
	switch( (*p) ) {
		case 2: goto st21;
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 47: goto tr266;
		case 58: goto tr279;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 65 ) {
		if ( 36 <= (*p) && (*p) <= 59 )
			goto st247;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st247;
	} else
		goto st247;
	goto st0;
st276:
	if ( ++p == pe )
		goto _test_eof276;
case 276:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st275;
		case 47: goto tr266;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st277;
	goto st0;
st277:
	if ( ++p == pe )
		goto _test_eof277;
case 277:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st275;
		case 47: goto tr266;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st274;
	goto st0;
st278:
	if ( ++p == pe )
		goto _test_eof278;
case 278:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st275;
		case 47: goto tr266;
		case 53: goto st279;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st274;
	goto st0;
st279:
	if ( ++p == pe )
		goto _test_eof279;
case 279:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st275;
		case 47: goto tr266;
		case 58: goto tr279;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 54 ) {
		if ( (*p) > 45 ) {
			if ( 48 <= (*p) && (*p) <= 53 )
				goto st274;
		} else if ( (*p) >= 36 )
			goto st247;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st247;
	goto st0;
st280:
	if ( ++p == pe )
		goto _test_eof280;
case 280:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st273;
		case 47: goto tr266;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st281;
	goto st0;
st281:
	if ( ++p == pe )
		goto _test_eof281;
case 281:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st273;
		case 47: goto tr266;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st272;
	goto st0;
st282:
	if ( ++p == pe )
		goto _test_eof282;
case 282:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st273;
		case 47: goto tr266;
		case 53: goto st283;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st272;
	goto st0;
st283:
	if ( ++p == pe )
		goto _test_eof283;
case 283:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st273;
		case 47: goto tr266;
		case 58: goto tr279;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 54 ) {
		if ( (*p) > 45 ) {
			if ( 48 <= (*p) && (*p) <= 53 )
				goto st272;
		} else if ( (*p) >= 36 )
			goto st247;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st247;
	goto st0;
tr258:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st284;
st284:
	if ( ++p == pe )
		goto _test_eof284;
case 284:
#line 5052 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st271;
		case 47: goto tr266;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st285;
	goto st0;
tr260:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st285;
st285:
	if ( ++p == pe )
		goto _test_eof285;
case 285:
#line 5087 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st271;
		case 47: goto tr266;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st270;
	goto st0;
tr259:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st286;
st286:
	if ( ++p == pe )
		goto _test_eof286;
case 286:
#line 5122 "src/uri.cpp"
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st271;
		case 47: goto tr266;
		case 53: goto st287;
		case 58: goto tr279;
		case 59: goto st247;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 45 )
			goto st247;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st270;
	goto st0;
st287:
	if ( ++p == pe )
		goto _test_eof287;
case 287:
	switch( (*p) ) {
		case 33: goto st247;
		case 35: goto tr265;
		case 37: goto st31;
		case 46: goto st271;
		case 47: goto tr266;
		case 58: goto tr279;
		case 61: goto st247;
		case 63: goto tr268;
		case 64: goto tr38;
		case 95: goto st247;
		case 126: goto st247;
	}
	if ( (*p) < 54 ) {
		if ( (*p) > 45 ) {
			if ( 48 <= (*p) && (*p) <= 53 )
				goto st270;
		} else if ( (*p) >= 36 )
			goto st247;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st247;
		} else if ( (*p) >= 65 )
			goto st247;
	} else
		goto st247;
	goto st0;
tr238:
#line 17 "src/uri.rl"
	{ mark = p; }
	goto st288;
st288:
	if ( ++p == pe )
		goto _test_eof288;
case 288:
#line 5190 "src/uri.cpp"
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr234;
		case 37: goto st3;
		case 43: goto st288;
		case 47: goto tr239;
		case 58: goto tr323;
		case 60: goto st0;
		case 62: goto st0;
		case 63: goto tr237;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 65 ) {
		if ( (*p) > 32 ) {
			if ( 45 <= (*p) && (*p) <= 57 )
				goto st288;
		} else if ( (*p) >= 0 )
			goto st0;
	} else if ( (*p) > 90 ) {
		if ( (*p) < 97 ) {
			if ( 91 <= (*p) && (*p) <= 94 )
				goto st0;
		} else if ( (*p) > 122 ) {
			if ( 123 <= (*p) && (*p) <= 125 )
				goto st0;
		} else
			goto st288;
	} else
		goto st288;
	goto st232;
tr323:
#line 20 "src/uri.rl"
	{
        uri->setScheme(std::string(mark, p - mark));
        mark = NULL;
    }
	goto st289;
st289:
	if ( ++p == pe )
		goto _test_eof289;
case 289:
#line 5233 "src/uri.cpp"
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr234;
		case 37: goto st229;
		case 47: goto tr236;
		case 60: goto st0;
		case 62: goto st0;
		case 63: goto tr237;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 91 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 94 ) {
		if ( 123 <= (*p) && (*p) <= 125 )
			goto st0;
	} else
		goto st0;
	goto st290;
st290:
	if ( ++p == pe )
		goto _test_eof290;
case 290:
	switch( (*p) ) {
		case 34: goto st0;
		case 35: goto tr234;
		case 37: goto st229;
		case 47: goto tr239;
		case 60: goto st0;
		case 62: goto st0;
		case 63: goto tr237;
		case 96: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 91 ) {
		if ( 0 <= (*p) && (*p) <= 32 )
			goto st0;
	} else if ( (*p) > 94 ) {
		if ( 123 <= (*p) && (*p) <= 125 )
			goto st0;
	} else
		goto st0;
	goto st290;
st229:
	if ( ++p == pe )
		goto _test_eof229;
case 229:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st230;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st230;
	} else
		goto st230;
	goto st0;
st230:
	if ( ++p == pe )
		goto _test_eof230;
case 230:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st290;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st290;
	} else
		goto st290;
	goto st0;
	}
	_test_eof232: cs = 232; goto _test_eof; 
	_test_eof233: cs = 233; goto _test_eof; 
	_test_eof234: cs = 234; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof235: cs = 235; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof236: cs = 236; goto _test_eof; 
	_test_eof237: cs = 237; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof238: cs = 238; goto _test_eof; 
	_test_eof239: cs = 239; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof240: cs = 240; goto _test_eof; 
	_test_eof241: cs = 241; goto _test_eof; 
	_test_eof242: cs = 242; goto _test_eof; 
	_test_eof243: cs = 243; goto _test_eof; 
	_test_eof244: cs = 244; goto _test_eof; 
	_test_eof245: cs = 245; goto _test_eof; 
	_test_eof246: cs = 246; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof247: cs = 247; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof248: cs = 248; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof249: cs = 249; goto _test_eof; 
	_test_eof250: cs = 250; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof251: cs = 251; goto _test_eof; 
	_test_eof252: cs = 252; goto _test_eof; 
	_test_eof253: cs = 253; goto _test_eof; 
	_test_eof254: cs = 254; goto _test_eof; 
	_test_eof255: cs = 255; goto _test_eof; 
	_test_eof256: cs = 256; goto _test_eof; 
	_test_eof257: cs = 257; goto _test_eof; 
	_test_eof258: cs = 258; goto _test_eof; 
	_test_eof259: cs = 259; goto _test_eof; 
	_test_eof260: cs = 260; goto _test_eof; 
	_test_eof261: cs = 261; goto _test_eof; 
	_test_eof262: cs = 262; goto _test_eof; 
	_test_eof263: cs = 263; goto _test_eof; 
	_test_eof264: cs = 264; goto _test_eof; 
	_test_eof265: cs = 265; goto _test_eof; 
	_test_eof266: cs = 266; goto _test_eof; 
	_test_eof267: cs = 267; goto _test_eof; 
	_test_eof268: cs = 268; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
	_test_eof45: cs = 45; goto _test_eof; 
	_test_eof46: cs = 46; goto _test_eof; 
	_test_eof47: cs = 47; goto _test_eof; 
	_test_eof48: cs = 48; goto _test_eof; 
	_test_eof49: cs = 49; goto _test_eof; 
	_test_eof50: cs = 50; goto _test_eof; 
	_test_eof51: cs = 51; goto _test_eof; 
	_test_eof52: cs = 52; goto _test_eof; 
	_test_eof53: cs = 53; goto _test_eof; 
	_test_eof54: cs = 54; goto _test_eof; 
	_test_eof55: cs = 55; goto _test_eof; 
	_test_eof56: cs = 56; goto _test_eof; 
	_test_eof57: cs = 57; goto _test_eof; 
	_test_eof58: cs = 58; goto _test_eof; 
	_test_eof59: cs = 59; goto _test_eof; 
	_test_eof60: cs = 60; goto _test_eof; 
	_test_eof61: cs = 61; goto _test_eof; 
	_test_eof62: cs = 62; goto _test_eof; 
	_test_eof63: cs = 63; goto _test_eof; 
	_test_eof64: cs = 64; goto _test_eof; 
	_test_eof65: cs = 65; goto _test_eof; 
	_test_eof66: cs = 66; goto _test_eof; 
	_test_eof67: cs = 67; goto _test_eof; 
	_test_eof68: cs = 68; goto _test_eof; 
	_test_eof69: cs = 69; goto _test_eof; 
	_test_eof70: cs = 70; goto _test_eof; 
	_test_eof71: cs = 71; goto _test_eof; 
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof82: cs = 82; goto _test_eof; 
	_test_eof83: cs = 83; goto _test_eof; 
	_test_eof84: cs = 84; goto _test_eof; 
	_test_eof85: cs = 85; goto _test_eof; 
	_test_eof86: cs = 86; goto _test_eof; 
	_test_eof87: cs = 87; goto _test_eof; 
	_test_eof88: cs = 88; goto _test_eof; 
	_test_eof89: cs = 89; goto _test_eof; 
	_test_eof90: cs = 90; goto _test_eof; 
	_test_eof91: cs = 91; goto _test_eof; 
	_test_eof92: cs = 92; goto _test_eof; 
	_test_eof93: cs = 93; goto _test_eof; 
	_test_eof94: cs = 94; goto _test_eof; 
	_test_eof95: cs = 95; goto _test_eof; 
	_test_eof96: cs = 96; goto _test_eof; 
	_test_eof97: cs = 97; goto _test_eof; 
	_test_eof98: cs = 98; goto _test_eof; 
	_test_eof99: cs = 99; goto _test_eof; 
	_test_eof100: cs = 100; goto _test_eof; 
	_test_eof101: cs = 101; goto _test_eof; 
	_test_eof102: cs = 102; goto _test_eof; 
	_test_eof103: cs = 103; goto _test_eof; 
	_test_eof104: cs = 104; goto _test_eof; 
	_test_eof105: cs = 105; goto _test_eof; 
	_test_eof106: cs = 106; goto _test_eof; 
	_test_eof107: cs = 107; goto _test_eof; 
	_test_eof108: cs = 108; goto _test_eof; 
	_test_eof109: cs = 109; goto _test_eof; 
	_test_eof110: cs = 110; goto _test_eof; 
	_test_eof111: cs = 111; goto _test_eof; 
	_test_eof112: cs = 112; goto _test_eof; 
	_test_eof113: cs = 113; goto _test_eof; 
	_test_eof114: cs = 114; goto _test_eof; 
	_test_eof115: cs = 115; goto _test_eof; 
	_test_eof116: cs = 116; goto _test_eof; 
	_test_eof117: cs = 117; goto _test_eof; 
	_test_eof118: cs = 118; goto _test_eof; 
	_test_eof119: cs = 119; goto _test_eof; 
	_test_eof120: cs = 120; goto _test_eof; 
	_test_eof121: cs = 121; goto _test_eof; 
	_test_eof122: cs = 122; goto _test_eof; 
	_test_eof123: cs = 123; goto _test_eof; 
	_test_eof124: cs = 124; goto _test_eof; 
	_test_eof125: cs = 125; goto _test_eof; 
	_test_eof126: cs = 126; goto _test_eof; 
	_test_eof127: cs = 127; goto _test_eof; 
	_test_eof128: cs = 128; goto _test_eof; 
	_test_eof129: cs = 129; goto _test_eof; 
	_test_eof130: cs = 130; goto _test_eof; 
	_test_eof131: cs = 131; goto _test_eof; 
	_test_eof132: cs = 132; goto _test_eof; 
	_test_eof133: cs = 133; goto _test_eof; 
	_test_eof134: cs = 134; goto _test_eof; 
	_test_eof135: cs = 135; goto _test_eof; 
	_test_eof136: cs = 136; goto _test_eof; 
	_test_eof137: cs = 137; goto _test_eof; 
	_test_eof138: cs = 138; goto _test_eof; 
	_test_eof139: cs = 139; goto _test_eof; 
	_test_eof140: cs = 140; goto _test_eof; 
	_test_eof141: cs = 141; goto _test_eof; 
	_test_eof142: cs = 142; goto _test_eof; 
	_test_eof143: cs = 143; goto _test_eof; 
	_test_eof144: cs = 144; goto _test_eof; 
	_test_eof145: cs = 145; goto _test_eof; 
	_test_eof146: cs = 146; goto _test_eof; 
	_test_eof147: cs = 147; goto _test_eof; 
	_test_eof148: cs = 148; goto _test_eof; 
	_test_eof149: cs = 149; goto _test_eof; 
	_test_eof150: cs = 150; goto _test_eof; 
	_test_eof151: cs = 151; goto _test_eof; 
	_test_eof152: cs = 152; goto _test_eof; 
	_test_eof153: cs = 153; goto _test_eof; 
	_test_eof154: cs = 154; goto _test_eof; 
	_test_eof155: cs = 155; goto _test_eof; 
	_test_eof156: cs = 156; goto _test_eof; 
	_test_eof157: cs = 157; goto _test_eof; 
	_test_eof158: cs = 158; goto _test_eof; 
	_test_eof159: cs = 159; goto _test_eof; 
	_test_eof160: cs = 160; goto _test_eof; 
	_test_eof161: cs = 161; goto _test_eof; 
	_test_eof162: cs = 162; goto _test_eof; 
	_test_eof163: cs = 163; goto _test_eof; 
	_test_eof164: cs = 164; goto _test_eof; 
	_test_eof165: cs = 165; goto _test_eof; 
	_test_eof166: cs = 166; goto _test_eof; 
	_test_eof167: cs = 167; goto _test_eof; 
	_test_eof168: cs = 168; goto _test_eof; 
	_test_eof169: cs = 169; goto _test_eof; 
	_test_eof170: cs = 170; goto _test_eof; 
	_test_eof171: cs = 171; goto _test_eof; 
	_test_eof172: cs = 172; goto _test_eof; 
	_test_eof173: cs = 173; goto _test_eof; 
	_test_eof174: cs = 174; goto _test_eof; 
	_test_eof175: cs = 175; goto _test_eof; 
	_test_eof176: cs = 176; goto _test_eof; 
	_test_eof177: cs = 177; goto _test_eof; 
	_test_eof178: cs = 178; goto _test_eof; 
	_test_eof179: cs = 179; goto _test_eof; 
	_test_eof180: cs = 180; goto _test_eof; 
	_test_eof181: cs = 181; goto _test_eof; 
	_test_eof182: cs = 182; goto _test_eof; 
	_test_eof183: cs = 183; goto _test_eof; 
	_test_eof184: cs = 184; goto _test_eof; 
	_test_eof185: cs = 185; goto _test_eof; 
	_test_eof186: cs = 186; goto _test_eof; 
	_test_eof187: cs = 187; goto _test_eof; 
	_test_eof188: cs = 188; goto _test_eof; 
	_test_eof189: cs = 189; goto _test_eof; 
	_test_eof190: cs = 190; goto _test_eof; 
	_test_eof191: cs = 191; goto _test_eof; 
	_test_eof192: cs = 192; goto _test_eof; 
	_test_eof193: cs = 193; goto _test_eof; 
	_test_eof194: cs = 194; goto _test_eof; 
	_test_eof195: cs = 195; goto _test_eof; 
	_test_eof196: cs = 196; goto _test_eof; 
	_test_eof197: cs = 197; goto _test_eof; 
	_test_eof198: cs = 198; goto _test_eof; 
	_test_eof199: cs = 199; goto _test_eof; 
	_test_eof200: cs = 200; goto _test_eof; 
	_test_eof201: cs = 201; goto _test_eof; 
	_test_eof202: cs = 202; goto _test_eof; 
	_test_eof203: cs = 203; goto _test_eof; 
	_test_eof204: cs = 204; goto _test_eof; 
	_test_eof205: cs = 205; goto _test_eof; 
	_test_eof206: cs = 206; goto _test_eof; 
	_test_eof207: cs = 207; goto _test_eof; 
	_test_eof208: cs = 208; goto _test_eof; 
	_test_eof209: cs = 209; goto _test_eof; 
	_test_eof210: cs = 210; goto _test_eof; 
	_test_eof211: cs = 211; goto _test_eof; 
	_test_eof212: cs = 212; goto _test_eof; 
	_test_eof213: cs = 213; goto _test_eof; 
	_test_eof214: cs = 214; goto _test_eof; 
	_test_eof215: cs = 215; goto _test_eof; 
	_test_eof216: cs = 216; goto _test_eof; 
	_test_eof217: cs = 217; goto _test_eof; 
	_test_eof218: cs = 218; goto _test_eof; 
	_test_eof219: cs = 219; goto _test_eof; 
	_test_eof220: cs = 220; goto _test_eof; 
	_test_eof221: cs = 221; goto _test_eof; 
	_test_eof222: cs = 222; goto _test_eof; 
	_test_eof223: cs = 223; goto _test_eof; 
	_test_eof224: cs = 224; goto _test_eof; 
	_test_eof225: cs = 225; goto _test_eof; 
	_test_eof226: cs = 226; goto _test_eof; 
	_test_eof227: cs = 227; goto _test_eof; 
	_test_eof228: cs = 228; goto _test_eof; 
	_test_eof269: cs = 269; goto _test_eof; 
	_test_eof270: cs = 270; goto _test_eof; 
	_test_eof271: cs = 271; goto _test_eof; 
	_test_eof272: cs = 272; goto _test_eof; 
	_test_eof273: cs = 273; goto _test_eof; 
	_test_eof274: cs = 274; goto _test_eof; 
	_test_eof275: cs = 275; goto _test_eof; 
	_test_eof276: cs = 276; goto _test_eof; 
	_test_eof277: cs = 277; goto _test_eof; 
	_test_eof278: cs = 278; goto _test_eof; 
	_test_eof279: cs = 279; goto _test_eof; 
	_test_eof280: cs = 280; goto _test_eof; 
	_test_eof281: cs = 281; goto _test_eof; 
	_test_eof282: cs = 282; goto _test_eof; 
	_test_eof283: cs = 283; goto _test_eof; 
	_test_eof284: cs = 284; goto _test_eof; 
	_test_eof285: cs = 285; goto _test_eof; 
	_test_eof286: cs = 286; goto _test_eof; 
	_test_eof287: cs = 287; goto _test_eof; 
	_test_eof288: cs = 288; goto _test_eof; 
	_test_eof289: cs = 289; goto _test_eof; 
	_test_eof290: cs = 290; goto _test_eof; 
	_test_eof229: cs = 229; goto _test_eof; 
	_test_eof230: cs = 230; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 235: 
	case 238: 
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 237: 
#line 48 "src/uri.rl"
	{
        uri->setQuery(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 234: 
#line 53 "src/uri.rl"
	{
        uri->setFragment(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 231: 
	case 232: 
	case 288: 
	case 289: 
	case 290: 
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 236: 
#line 17 "src/uri.rl"
	{ mark = p; }
#line 48 "src/uri.rl"
	{
        uri->setQuery(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 233: 
#line 17 "src/uri.rl"
	{ mark = p; }
#line 53 "src/uri.rl"
	{
        uri->setFragment(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 239: 
	case 249: 
#line 17 "src/uri.rl"
	{ mark = p; }
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 241: 
	case 248: 
#line 17 "src/uri.rl"
	{ mark = p; }
#line 40 "src/uri.rl"
	{
        if (p != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 240: 
	case 243: 
	case 244: 
	case 245: 
	case 246: 
	case 247: 
	case 250: 
	case 251: 
	case 252: 
	case 253: 
	case 254: 
	case 255: 
	case 256: 
	case 257: 
	case 258: 
	case 259: 
	case 260: 
	case 261: 
	case 262: 
	case 263: 
	case 264: 
	case 265: 
	case 266: 
	case 267: 
	case 268: 
	case 270: 
	case 271: 
	case 272: 
	case 273: 
	case 274: 
	case 275: 
	case 276: 
	case 277: 
	case 278: 
	case 279: 
	case 280: 
	case 281: 
	case 282: 
	case 283: 
	case 284: 
	case 285: 
	case 286: 
	case 287: 
#line 30 "src/uri.rl"
	{
        uri->setHost(std::string(mark, p - mark));
        mark = NULL;
    }
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
	case 242: 
	case 269: 
#line 40 "src/uri.rl"
	{
        if (p != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
#line 17 "src/uri.rl"
	{ mark = p; }
#line 35 "src/uri.rl"
	{
        uri->setPath(std::string(mark, p - mark));
        mark = NULL;
    }
	break;
#line 5759 "src/uri.cpp"
	}
	}

	_out: {}
	}

#line 132 "src/uri.rl"

    if (cs >= uri_parser_first_final)
    {
        return uri;
    }

    return nullptr;
}

Address::ptr Uri::createAddress() const
{
    std::string host = m_host + ":" + std::to_string(getPort());
    auto addr = Address::getAddrInfo(m_host);
    if (addr)
        addr->setPort(getPort());

    return addr;
}

bool Uri::isDefaultPort() const
{
    if (m_port == 0)
        return true;
    else if (m_scheme == "http" || m_scheme == "ws")
        return m_port == 80;
    else if (m_scheme == "https" || m_scheme == "wss")
        return m_port == 443;
    return false;
}

std::string Uri::toString() const
{
    std::stringstream ss;
    ss << m_scheme
       << (m_scheme.empty() ? "" : ":")
       << (m_host.empty() ? "" : "//")
       << m_userinfo
       << (m_userinfo.empty() ? "" : "@")
       << m_host
       << (isDefaultPort() ? "" : ":" + std::to_string(m_port))
       << getPath()
       << (m_query.empty() ? "" : "?")
       << m_query
       << (m_fragment.empty() ? "" : "#")
       << m_fragment;

    return ss.str();
}

}
