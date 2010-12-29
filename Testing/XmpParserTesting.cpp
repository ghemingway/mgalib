/*** Included Header Files ***/
#include "XmpParserTesting.h"


TEST(XmpParserTest,Create)
{
	EXPECT_EQ( XmpParser::Parse("MetaGME.xmp", "MetaGME-Parser.mta"), S_OK );
}