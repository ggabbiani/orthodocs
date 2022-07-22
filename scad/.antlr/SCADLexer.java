// Generated from /home/giampa/projects/test2/scad/SCADLexer.g4 by ANTLR 4.9.2
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class SCADLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.9.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		COLON=1, SEMICOLON=2, LEFT_CURLY_BRACKET=3, RIGHT_CURLY_BRACKET=4, LEFT_PARENTHESIS=5, 
		RIGHT_PARENTHESIS=6, LEFT_SQUARE_BRACKET=7, RIGHT_SQUARE_BRACKET=8, COMMA=9, 
		EQUAL=10, EXCLAMATION_MARK=11, QUESTION_MARK=12, MINUS=13, PLUS=14, SLASH=15, 
		PERCENT=16, GREATER_THEN=17, LESS_THEN=18, STAR=19, LET=20, FOR=21, IF=22, 
		FUNCTION=23, MODULE=24, INCLUDE=25, USE=26, TRUE=27, FALSE=28, UNDEF=29, 
		GE=30, EQ=31, NE=32, LE=33, AND=34, OR=35, BLOCKCOMMENT=36, LINECOMMENT=37, 
		WS=38, NL=39, SIGN=40, NUMBER=41, DIGIT_SEQUENCE=42, ID=43, PATH_COMPONENT=44, 
		FILE=45, STRING=46;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"COLON", "SEMICOLON", "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET", "LEFT_PARENTHESIS", 
			"RIGHT_PARENTHESIS", "LEFT_SQUARE_BRACKET", "RIGHT_SQUARE_BRACKET", "COMMA", 
			"EQUAL", "EXCLAMATION_MARK", "QUESTION_MARK", "MINUS", "PLUS", "SLASH", 
			"PERCENT", "GREATER_THEN", "LESS_THEN", "STAR", "LET", "FOR", "IF", "FUNCTION", 
			"MODULE", "INCLUDE", "USE", "TRUE", "FALSE", "UNDEF", "GE", "EQ", "NE", 
			"LE", "AND", "OR", "BLOCKCOMMENT", "LINECOMMENT", "WS", "NL", "ALPHA", 
			"DIGIT", "NONDIGIT", "INT", "FP", "FRACTIONAL", "EXPONENT", "SIGN", "NUMBER", 
			"DIGIT_SEQUENCE", "ID", "PATH_COMPONENT", "FILE", "ESCAPE_SEQUENCE", 
			"SIMPLE_ESCAPE_SEQUENCE", "STRING", "CHAR_SEQUENCE", "CHAR", "UNICODE", 
			"HEX_DIGIT"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "':'", "';'", "'{'", "'}'", "'('", "')'", "'['", "']'", "','", 
			"'='", "'!'", "'?'", "'-'", "'+'", "'/'", "'%'", "'>'", "'<'", "'*'", 
			"'let'", "'for'", "'if'", "'function'", "'module'", "'include'", "'use'", 
			"'true'", "'false'", "'undef'", "'>='", "'=='", "'!='", "'<='", "'&&'", 
			"'||'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "COLON", "SEMICOLON", "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET", 
			"LEFT_PARENTHESIS", "RIGHT_PARENTHESIS", "LEFT_SQUARE_BRACKET", "RIGHT_SQUARE_BRACKET", 
			"COMMA", "EQUAL", "EXCLAMATION_MARK", "QUESTION_MARK", "MINUS", "PLUS", 
			"SLASH", "PERCENT", "GREATER_THEN", "LESS_THEN", "STAR", "LET", "FOR", 
			"IF", "FUNCTION", "MODULE", "INCLUDE", "USE", "TRUE", "FALSE", "UNDEF", 
			"GE", "EQ", "NE", "LE", "AND", "OR", "BLOCKCOMMENT", "LINECOMMENT", "WS", 
			"NL", "SIGN", "NUMBER", "DIGIT_SEQUENCE", "ID", "PATH_COMPONENT", "FILE", 
			"STRING"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}


	public SCADLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "SCADLexer.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2\60\u0182\b\1\4\2"+
		"\t\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4"+
		"\13\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22"+
		"\t\22\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31"+
		"\t\31\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\4\37\t\37\4 \t"+
		" \4!\t!\4\"\t\"\4#\t#\4$\t$\4%\t%\4&\t&\4\'\t\'\4(\t(\4)\t)\4*\t*\4+\t"+
		"+\4,\t,\4-\t-\4.\t.\4/\t/\4\60\t\60\4\61\t\61\4\62\t\62\4\63\t\63\4\64"+
		"\t\64\4\65\t\65\4\66\t\66\4\67\t\67\48\t8\49\t9\4:\t:\4;\t;\4<\t<\3\2"+
		"\3\2\3\3\3\3\3\4\3\4\3\5\3\5\3\6\3\6\3\7\3\7\3\b\3\b\3\t\3\t\3\n\3\n\3"+
		"\13\3\13\3\f\3\f\3\r\3\r\3\16\3\16\3\17\3\17\3\20\3\20\3\21\3\21\3\22"+
		"\3\22\3\23\3\23\3\24\3\24\3\25\3\25\3\25\3\25\3\26\3\26\3\26\3\26\3\27"+
		"\3\27\3\27\3\30\3\30\3\30\3\30\3\30\3\30\3\30\3\30\3\30\3\31\3\31\3\31"+
		"\3\31\3\31\3\31\3\31\3\32\3\32\3\32\3\32\3\32\3\32\3\32\3\32\3\33\3\33"+
		"\3\33\3\33\3\34\3\34\3\34\3\34\3\34\3\35\3\35\3\35\3\35\3\35\3\35\3\36"+
		"\3\36\3\36\3\36\3\36\3\36\3\37\3\37\3\37\3 \3 \3 \3!\3!\3!\3\"\3\"\3\""+
		"\3#\3#\3#\3$\3$\3$\3%\3%\3%\3%\7%\u00ee\n%\f%\16%\u00f1\13%\3%\3%\3%\3"+
		"%\3%\3&\3&\3&\3&\7&\u00fc\n&\f&\16&\u00ff\13&\3&\3&\3\'\6\'\u0104\n\'"+
		"\r\'\16\'\u0105\3\'\3\'\3(\3(\5(\u010c\n(\3(\5(\u010f\n(\3(\3(\3)\3)\3"+
		"*\3*\3+\3+\3,\5,\u011a\n,\3,\6,\u011d\n,\r,\16,\u011e\3-\3-\5-\u0123\n"+
		"-\3-\3-\3-\5-\u0128\n-\3.\5.\u012b\n.\3.\5.\u012e\n.\3.\3.\3.\3.\3.\5"+
		".\u0135\n.\3/\3/\5/\u0139\n/\3/\3/\3\60\3\60\3\61\3\61\5\61\u0141\n\61"+
		"\3\62\6\62\u0144\n\62\r\62\16\62\u0145\3\63\5\63\u0149\n\63\3\63\6\63"+
		"\u014c\n\63\r\63\16\63\u014d\3\64\3\64\3\64\3\64\6\64\u0154\n\64\r\64"+
		"\16\64\u0155\3\65\3\65\3\65\3\65\7\65\u015c\n\65\f\65\16\65\u015f\13\65"+
		"\3\65\3\65\3\66\3\66\5\66\u0165\n\66\3\67\3\67\3\67\38\38\58\u016c\n8"+
		"\38\38\39\69\u0171\n9\r9\169\u0172\3:\3:\5:\u0177\n:\3;\3;\3;\3;\3;\3"+
		";\3;\3;\3<\3<\3\u00ef\2=\3\3\5\4\7\5\t\6\13\7\r\b\17\t\21\n\23\13\25\f"+
		"\27\r\31\16\33\17\35\20\37\21!\22#\23%\24\'\25)\26+\27-\30/\31\61\32\63"+
		"\33\65\34\67\359\36;\37= ?!A\"C#E$G%I&K\'M(O)Q\2S\2U\2W\2Y\2[\2]\2_*a"+
		"+c,e-g.i/k\2m\2o\60q\2s\2u\2w\2\3\2\16\4\2\f\f\17\17\4\2\13\13\"\"\4\2"+
		"C\\c|\3\2\62;\5\2C\\aac|\4\2GGgg\4\2--//\6\2\62;C\\aac|\4\2\60\60aa\7"+
		"\2$$^^ppttvv\6\2\f\f\17\17$$^^\5\2\62;CHch\2\u018e\2\3\3\2\2\2\2\5\3\2"+
		"\2\2\2\7\3\2\2\2\2\t\3\2\2\2\2\13\3\2\2\2\2\r\3\2\2\2\2\17\3\2\2\2\2\21"+
		"\3\2\2\2\2\23\3\2\2\2\2\25\3\2\2\2\2\27\3\2\2\2\2\31\3\2\2\2\2\33\3\2"+
		"\2\2\2\35\3\2\2\2\2\37\3\2\2\2\2!\3\2\2\2\2#\3\2\2\2\2%\3\2\2\2\2\'\3"+
		"\2\2\2\2)\3\2\2\2\2+\3\2\2\2\2-\3\2\2\2\2/\3\2\2\2\2\61\3\2\2\2\2\63\3"+
		"\2\2\2\2\65\3\2\2\2\2\67\3\2\2\2\29\3\2\2\2\2;\3\2\2\2\2=\3\2\2\2\2?\3"+
		"\2\2\2\2A\3\2\2\2\2C\3\2\2\2\2E\3\2\2\2\2G\3\2\2\2\2I\3\2\2\2\2K\3\2\2"+
		"\2\2M\3\2\2\2\2O\3\2\2\2\2_\3\2\2\2\2a\3\2\2\2\2c\3\2\2\2\2e\3\2\2\2\2"+
		"g\3\2\2\2\2i\3\2\2\2\2o\3\2\2\2\3y\3\2\2\2\5{\3\2\2\2\7}\3\2\2\2\t\177"+
		"\3\2\2\2\13\u0081\3\2\2\2\r\u0083\3\2\2\2\17\u0085\3\2\2\2\21\u0087\3"+
		"\2\2\2\23\u0089\3\2\2\2\25\u008b\3\2\2\2\27\u008d\3\2\2\2\31\u008f\3\2"+
		"\2\2\33\u0091\3\2\2\2\35\u0093\3\2\2\2\37\u0095\3\2\2\2!\u0097\3\2\2\2"+
		"#\u0099\3\2\2\2%\u009b\3\2\2\2\'\u009d\3\2\2\2)\u009f\3\2\2\2+\u00a3\3"+
		"\2\2\2-\u00a7\3\2\2\2/\u00aa\3\2\2\2\61\u00b3\3\2\2\2\63\u00ba\3\2\2\2"+
		"\65\u00c2\3\2\2\2\67\u00c6\3\2\2\29\u00cb\3\2\2\2;\u00d1\3\2\2\2=\u00d7"+
		"\3\2\2\2?\u00da\3\2\2\2A\u00dd\3\2\2\2C\u00e0\3\2\2\2E\u00e3\3\2\2\2G"+
		"\u00e6\3\2\2\2I\u00e9\3\2\2\2K\u00f7\3\2\2\2M\u0103\3\2\2\2O\u010e\3\2"+
		"\2\2Q\u0112\3\2\2\2S\u0114\3\2\2\2U\u0116\3\2\2\2W\u0119\3\2\2\2Y\u0127"+
		"\3\2\2\2[\u012a\3\2\2\2]\u0136\3\2\2\2_\u013c\3\2\2\2a\u0140\3\2\2\2c"+
		"\u0143\3\2\2\2e\u0148\3\2\2\2g\u0153\3\2\2\2i\u0157\3\2\2\2k\u0164\3\2"+
		"\2\2m\u0166\3\2\2\2o\u0169\3\2\2\2q\u0170\3\2\2\2s\u0176\3\2\2\2u\u0178"+
		"\3\2\2\2w\u0180\3\2\2\2yz\7<\2\2z\4\3\2\2\2{|\7=\2\2|\6\3\2\2\2}~\7}\2"+
		"\2~\b\3\2\2\2\177\u0080\7\177\2\2\u0080\n\3\2\2\2\u0081\u0082\7*\2\2\u0082"+
		"\f\3\2\2\2\u0083\u0084\7+\2\2\u0084\16\3\2\2\2\u0085\u0086\7]\2\2\u0086"+
		"\20\3\2\2\2\u0087\u0088\7_\2\2\u0088\22\3\2\2\2\u0089\u008a\7.\2\2\u008a"+
		"\24\3\2\2\2\u008b\u008c\7?\2\2\u008c\26\3\2\2\2\u008d\u008e\7#\2\2\u008e"+
		"\30\3\2\2\2\u008f\u0090\7A\2\2\u0090\32\3\2\2\2\u0091\u0092\7/\2\2\u0092"+
		"\34\3\2\2\2\u0093\u0094\7-\2\2\u0094\36\3\2\2\2\u0095\u0096\7\61\2\2\u0096"+
		" \3\2\2\2\u0097\u0098\7\'\2\2\u0098\"\3\2\2\2\u0099\u009a\7@\2\2\u009a"+
		"$\3\2\2\2\u009b\u009c\7>\2\2\u009c&\3\2\2\2\u009d\u009e\7,\2\2\u009e("+
		"\3\2\2\2\u009f\u00a0\7n\2\2\u00a0\u00a1\7g\2\2\u00a1\u00a2\7v\2\2\u00a2"+
		"*\3\2\2\2\u00a3\u00a4\7h\2\2\u00a4\u00a5\7q\2\2\u00a5\u00a6\7t\2\2\u00a6"+
		",\3\2\2\2\u00a7\u00a8\7k\2\2\u00a8\u00a9\7h\2\2\u00a9.\3\2\2\2\u00aa\u00ab"+
		"\7h\2\2\u00ab\u00ac\7w\2\2\u00ac\u00ad\7p\2\2\u00ad\u00ae\7e\2\2\u00ae"+
		"\u00af\7v\2\2\u00af\u00b0\7k\2\2\u00b0\u00b1\7q\2\2\u00b1\u00b2\7p\2\2"+
		"\u00b2\60\3\2\2\2\u00b3\u00b4\7o\2\2\u00b4\u00b5\7q\2\2\u00b5\u00b6\7"+
		"f\2\2\u00b6\u00b7\7w\2\2\u00b7\u00b8\7n\2\2\u00b8\u00b9\7g\2\2\u00b9\62"+
		"\3\2\2\2\u00ba\u00bb\7k\2\2\u00bb\u00bc\7p\2\2\u00bc\u00bd\7e\2\2\u00bd"+
		"\u00be\7n\2\2\u00be\u00bf\7w\2\2\u00bf\u00c0\7f\2\2\u00c0\u00c1\7g\2\2"+
		"\u00c1\64\3\2\2\2\u00c2\u00c3\7w\2\2\u00c3\u00c4\7u\2\2\u00c4\u00c5\7"+
		"g\2\2\u00c5\66\3\2\2\2\u00c6\u00c7\7v\2\2\u00c7\u00c8\7t\2\2\u00c8\u00c9"+
		"\7w\2\2\u00c9\u00ca\7g\2\2\u00ca8\3\2\2\2\u00cb\u00cc\7h\2\2\u00cc\u00cd"+
		"\7c\2\2\u00cd\u00ce\7n\2\2\u00ce\u00cf\7u\2\2\u00cf\u00d0\7g\2\2\u00d0"+
		":\3\2\2\2\u00d1\u00d2\7w\2\2\u00d2\u00d3\7p\2\2\u00d3\u00d4\7f\2\2\u00d4"+
		"\u00d5\7g\2\2\u00d5\u00d6\7h\2\2\u00d6<\3\2\2\2\u00d7\u00d8\7@\2\2\u00d8"+
		"\u00d9\7?\2\2\u00d9>\3\2\2\2\u00da\u00db\7?\2\2\u00db\u00dc\7?\2\2\u00dc"+
		"@\3\2\2\2\u00dd\u00de\7#\2\2\u00de\u00df\7?\2\2\u00dfB\3\2\2\2\u00e0\u00e1"+
		"\7>\2\2\u00e1\u00e2\7?\2\2\u00e2D\3\2\2\2\u00e3\u00e4\7(\2\2\u00e4\u00e5"+
		"\7(\2\2\u00e5F\3\2\2\2\u00e6\u00e7\7~\2\2\u00e7\u00e8\7~\2\2\u00e8H\3"+
		"\2\2\2\u00e9\u00ea\7\61\2\2\u00ea\u00eb\7,\2\2\u00eb\u00ef\3\2\2\2\u00ec"+
		"\u00ee\13\2\2\2\u00ed\u00ec\3\2\2\2\u00ee\u00f1\3\2\2\2\u00ef\u00f0\3"+
		"\2\2\2\u00ef\u00ed\3\2\2\2\u00f0\u00f2\3\2\2\2\u00f1\u00ef\3\2\2\2\u00f2"+
		"\u00f3\7,\2\2\u00f3\u00f4\7\61\2\2\u00f4\u00f5\3\2\2\2\u00f5\u00f6\b%"+
		"\2\2\u00f6J\3\2\2\2\u00f7\u00f8\7\61\2\2\u00f8\u00f9\7\61\2\2\u00f9\u00fd"+
		"\3\2\2\2\u00fa\u00fc\n\2\2\2\u00fb\u00fa\3\2\2\2\u00fc\u00ff\3\2\2\2\u00fd"+
		"\u00fb\3\2\2\2\u00fd\u00fe\3\2\2\2\u00fe\u0100\3\2\2\2\u00ff\u00fd\3\2"+
		"\2\2\u0100\u0101\b&\2\2\u0101L\3\2\2\2\u0102\u0104\t\3\2\2\u0103\u0102"+
		"\3\2\2\2\u0104\u0105\3\2\2\2\u0105\u0103\3\2\2\2\u0105\u0106\3\2\2\2\u0106"+
		"\u0107\3\2\2\2\u0107\u0108\b\'\2\2\u0108N\3\2\2\2\u0109\u010b\7\17\2\2"+
		"\u010a\u010c\7\f\2\2\u010b\u010a\3\2\2\2\u010b\u010c\3\2\2\2\u010c\u010f"+
		"\3\2\2\2\u010d\u010f\7\f\2\2\u010e\u0109\3\2\2\2\u010e\u010d\3\2\2\2\u010f"+
		"\u0110\3\2\2\2\u0110\u0111\b(\2\2\u0111P\3\2\2\2\u0112\u0113\t\4\2\2\u0113"+
		"R\3\2\2\2\u0114\u0115\t\5\2\2\u0115T\3\2\2\2\u0116\u0117\t\6\2\2\u0117"+
		"V\3\2\2\2\u0118\u011a\5_\60\2\u0119\u0118\3\2\2\2\u0119\u011a\3\2\2\2"+
		"\u011a\u011c\3\2\2\2\u011b\u011d\5S*\2\u011c\u011b\3\2\2\2\u011d\u011e"+
		"\3\2\2\2\u011e\u011c\3\2\2\2\u011e\u011f\3\2\2\2\u011fX\3\2\2\2\u0120"+
		"\u0122\5[.\2\u0121\u0123\5]/\2\u0122\u0121\3\2\2\2\u0122\u0123\3\2\2\2"+
		"\u0123\u0128\3\2\2\2\u0124\u0125\5c\62\2\u0125\u0126\5]/\2\u0126\u0128"+
		"\3\2\2\2\u0127\u0120\3\2\2\2\u0127\u0124\3\2\2\2\u0128Z\3\2\2\2\u0129"+
		"\u012b\5_\60\2\u012a\u0129\3\2\2\2\u012a\u012b\3\2\2\2\u012b\u0134\3\2"+
		"\2\2\u012c\u012e\5c\62\2\u012d\u012c\3\2\2\2\u012d\u012e\3\2\2\2\u012e"+
		"\u012f\3\2\2\2\u012f\u0130\7\60\2\2\u0130\u0135\5c\62\2\u0131\u0132\5"+
		"c\62\2\u0132\u0133\7\60\2\2\u0133\u0135\3\2\2\2\u0134\u012d\3\2\2\2\u0134"+
		"\u0131\3\2\2\2\u0135\\\3\2\2\2\u0136\u0138\t\7\2\2\u0137\u0139\5_\60\2"+
		"\u0138\u0137\3\2\2\2\u0138\u0139\3\2\2\2\u0139\u013a\3\2\2\2\u013a\u013b"+
		"\5c\62\2\u013b^\3\2\2\2\u013c\u013d\t\b\2\2\u013d`\3\2\2\2\u013e\u0141"+
		"\5W,\2\u013f\u0141\5Y-\2\u0140\u013e\3\2\2\2\u0140\u013f\3\2\2\2\u0141"+
		"b\3\2\2\2\u0142\u0144\5S*\2\u0143\u0142\3\2\2\2\u0144\u0145\3\2\2\2\u0145"+
		"\u0143\3\2\2\2\u0145\u0146\3\2\2\2\u0146d\3\2\2\2\u0147\u0149\7&\2\2\u0148"+
		"\u0147\3\2\2\2\u0148\u0149\3\2\2\2\u0149\u014b\3\2\2\2\u014a\u014c\t\t"+
		"\2\2\u014b\u014a\3\2\2\2\u014c\u014d\3\2\2\2\u014d\u014b\3\2\2\2\u014d"+
		"\u014e\3\2\2\2\u014ef\3\2\2\2\u014f\u0154\5Q)\2\u0150\u0154\5S*\2\u0151"+
		"\u0154\5\33\16\2\u0152\u0154\t\n\2\2\u0153\u014f\3\2\2\2\u0153\u0150\3"+
		"\2\2\2\u0153\u0151\3\2\2\2\u0153\u0152\3\2\2\2\u0154\u0155\3\2\2\2\u0155"+
		"\u0153\3\2\2\2\u0155\u0156\3\2\2\2\u0156h\3\2\2\2\u0157\u0158\7>\2\2\u0158"+
		"\u015d\5g\64\2\u0159\u015a\7\61\2\2\u015a\u015c\5g\64\2\u015b\u0159\3"+
		"\2\2\2\u015c\u015f\3\2\2\2\u015d\u015b\3\2\2\2\u015d\u015e\3\2\2\2\u015e"+
		"\u0160\3\2\2\2\u015f\u015d\3\2\2\2\u0160\u0161\7@\2\2\u0161j\3\2\2\2\u0162"+
		"\u0165\5m\67\2\u0163\u0165\5u;\2\u0164\u0162\3\2\2\2\u0164\u0163\3\2\2"+
		"\2\u0165l\3\2\2\2\u0166\u0167\7^\2\2\u0167\u0168\t\13\2\2\u0168n\3\2\2"+
		"\2\u0169\u016b\7$\2\2\u016a\u016c\5q9\2\u016b\u016a\3\2\2\2\u016b\u016c"+
		"\3\2\2\2\u016c\u016d\3\2\2\2\u016d\u016e\7$\2\2\u016ep\3\2\2\2\u016f\u0171"+
		"\5s:\2\u0170\u016f\3\2\2\2\u0171\u0172\3\2\2\2\u0172\u0170\3\2\2\2\u0172"+
		"\u0173\3\2\2\2\u0173r\3\2\2\2\u0174\u0177\n\f\2\2\u0175\u0177\5k\66\2"+
		"\u0176\u0174\3\2\2\2\u0176\u0175\3\2\2\2\u0177t\3\2\2\2\u0178\u0179\7"+
		"^\2\2\u0179\u017a\7w\2\2\u017a\u017b\3\2\2\2\u017b\u017c\5w<\2\u017c\u017d"+
		"\5w<\2\u017d\u017e\5w<\2\u017e\u017f\5w<\2\u017fv\3\2\2\2\u0180\u0181"+
		"\t\r\2\2\u0181x\3\2\2\2\33\2\u00ef\u00fd\u0105\u010b\u010e\u0119\u011e"+
		"\u0122\u0127\u012a\u012d\u0134\u0138\u0140\u0145\u0148\u014d\u0153\u0155"+
		"\u015d\u0164\u016b\u0172\u0176\3\b\2\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}