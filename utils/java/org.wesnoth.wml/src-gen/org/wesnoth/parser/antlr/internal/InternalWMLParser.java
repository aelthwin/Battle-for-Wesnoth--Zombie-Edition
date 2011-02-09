package org.wesnoth.parser.antlr.internal; 

import java.io.InputStream;
import org.eclipse.xtext.*;
import org.eclipse.xtext.parser.*;
import org.eclipse.xtext.parser.impl.*;
import org.eclipse.xtext.parsetree.*;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.parser.antlr.AbstractInternalAntlrParser;
import org.eclipse.xtext.parser.antlr.XtextTokenStream;
import org.eclipse.xtext.parser.antlr.XtextTokenStream.HiddenTokens;
import org.eclipse.xtext.parser.antlr.AntlrDatatypeRuleToken;
import org.eclipse.xtext.conversion.ValueConverterException;
import org.wesnoth.services.WMLGrammarAccess;



import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

@SuppressWarnings("all")
public class InternalWMLParser extends AbstractInternalAntlrParser {
    public static final String[] tokenNames = new String[] {
        "<invalid>", "<EOR>", "<DOWN>", "<UP>", "RULE_ID", "RULE_EOL", "RULE_SL_COMMENT", "RULE_LUA_CODE", "RULE_DEFINE", "RULE_ENDDEF", "RULE_IFDEF", "RULE_IFNDEF", "RULE_IFHAVE", "RULE_IFNHAVE", "RULE_ELSE", "RULE_ENDIF", "RULE_TEXTDOMAIN", "RULE_STRING", "RULE_ANY_OTHER", "RULE_WS", "'['", "'+'", "']'", "'[/'", "'='", "'{'", "'./'", "'~'", "'}'", "'_'", "'.'", "'$'", "'/'", "'('", "')'"
    };
    public static final int RULE_LUA_CODE=7;
    public static final int RULE_ID=4;
    public static final int RULE_IFDEF=10;
    public static final int RULE_ANY_OTHER=18;
    public static final int RULE_IFNDEF=11;
    public static final int RULE_EOL=5;
    public static final int RULE_TEXTDOMAIN=16;
    public static final int RULE_IFNHAVE=13;
    public static final int RULE_SL_COMMENT=6;
    public static final int EOF=-1;
    public static final int RULE_STRING=17;
    public static final int RULE_ENDIF=15;
    public static final int RULE_DEFINE=8;
    public static final int RULE_ENDDEF=9;
    public static final int RULE_IFHAVE=12;
    public static final int RULE_WS=19;
    public static final int RULE_ELSE=14;

        public InternalWMLParser(TokenStream input) {
            super(input);
        }
        

    public String[] getTokenNames() { return tokenNames; }
    public String getGrammarFileName() { return "../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g"; }



     	private WMLGrammarAccess grammarAccess;
     	
        public InternalWMLParser(TokenStream input, IAstFactory factory, WMLGrammarAccess grammarAccess) {
            this(input);
            this.factory = factory;
            registerRules(grammarAccess.getGrammar());
            this.grammarAccess = grammarAccess;
        }
        
        @Override
        protected InputStream getTokenFile() {
        	ClassLoader classLoader = getClass().getClassLoader();
        	return classLoader.getResourceAsStream("org/wesnoth/parser/antlr/internal/InternalWML.tokens");
        }
        
        @Override
        protected String getFirstRuleName() {
        	return "WMLRoot";	
       	}
       	
       	@Override
       	protected WMLGrammarAccess getGrammarAccess() {
       		return grammarAccess;
       	}



    // $ANTLR start entryRuleWMLRoot
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:77:1: entryRuleWMLRoot returns [EObject current=null] : iv_ruleWMLRoot= ruleWMLRoot EOF ;
    public final EObject entryRuleWMLRoot() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLRoot = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:78:2: (iv_ruleWMLRoot= ruleWMLRoot EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:79:2: iv_ruleWMLRoot= ruleWMLRoot EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLRootRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLRoot_in_entryRuleWMLRoot75);
            iv_ruleWMLRoot=ruleWMLRoot();
            _fsp--;

             current =iv_ruleWMLRoot; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLRoot85); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLRoot


    // $ANTLR start ruleWMLRoot
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:86:1: ruleWMLRoot returns [EObject current=null] : ( ( (lv_Tags_0_0= ruleWMLTag ) ) | ( (lv_MacroCalls_1_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_2_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_3_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_4_0= ruleWMLPreprocIF ) ) )* ;
    public final EObject ruleWMLRoot() throws RecognitionException {
        EObject current = null;

        EObject lv_Tags_0_0 = null;

        EObject lv_MacroCalls_1_0 = null;

        EObject lv_MacroDefines_2_0 = null;

        EObject lv_Textdomains_3_0 = null;

        EObject lv_IfDefs_4_0 = null;


         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:91:6: ( ( ( (lv_Tags_0_0= ruleWMLTag ) ) | ( (lv_MacroCalls_1_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_2_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_3_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_4_0= ruleWMLPreprocIF ) ) )* )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:92:1: ( ( (lv_Tags_0_0= ruleWMLTag ) ) | ( (lv_MacroCalls_1_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_2_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_3_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_4_0= ruleWMLPreprocIF ) ) )*
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:92:1: ( ( (lv_Tags_0_0= ruleWMLTag ) ) | ( (lv_MacroCalls_1_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_2_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_3_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_4_0= ruleWMLPreprocIF ) ) )*
            loop1:
            do {
                int alt1=6;
                switch ( input.LA(1) ) {
                case 20:
                    {
                    alt1=1;
                    }
                    break;
                case 25:
                    {
                    alt1=2;
                    }
                    break;
                case RULE_DEFINE:
                    {
                    alt1=3;
                    }
                    break;
                case RULE_TEXTDOMAIN:
                    {
                    alt1=4;
                    }
                    break;
                case RULE_IFDEF:
                case RULE_IFNDEF:
                case RULE_IFHAVE:
                case RULE_IFNHAVE:
                    {
                    alt1=5;
                    }
                    break;

                }

                switch (alt1) {
            	case 1 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:92:2: ( (lv_Tags_0_0= ruleWMLTag ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:92:2: ( (lv_Tags_0_0= ruleWMLTag ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:93:1: (lv_Tags_0_0= ruleWMLTag )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:93:1: (lv_Tags_0_0= ruleWMLTag )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:94:3: lv_Tags_0_0= ruleWMLTag
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLRootAccess().getTagsWMLTagParserRuleCall_0_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLTag_in_ruleWMLRoot131);
            	    lv_Tags_0_0=ruleWMLTag();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLRootRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Tags",
            	    	        		lv_Tags_0_0, 
            	    	        		"WMLTag", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 2 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:117:6: ( (lv_MacroCalls_1_0= ruleWMLMacroCall ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:117:6: ( (lv_MacroCalls_1_0= ruleWMLMacroCall ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:118:1: (lv_MacroCalls_1_0= ruleWMLMacroCall )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:118:1: (lv_MacroCalls_1_0= ruleWMLMacroCall )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:119:3: lv_MacroCalls_1_0= ruleWMLMacroCall
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLRootAccess().getMacroCallsWMLMacroCallParserRuleCall_1_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroCall_in_ruleWMLRoot158);
            	    lv_MacroCalls_1_0=ruleWMLMacroCall();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLRootRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"MacroCalls",
            	    	        		lv_MacroCalls_1_0, 
            	    	        		"WMLMacroCall", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 3 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:142:6: ( (lv_MacroDefines_2_0= ruleWMLMacroDefine ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:142:6: ( (lv_MacroDefines_2_0= ruleWMLMacroDefine ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:143:1: (lv_MacroDefines_2_0= ruleWMLMacroDefine )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:143:1: (lv_MacroDefines_2_0= ruleWMLMacroDefine )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:144:3: lv_MacroDefines_2_0= ruleWMLMacroDefine
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLRootAccess().getMacroDefinesWMLMacroDefineParserRuleCall_2_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroDefine_in_ruleWMLRoot185);
            	    lv_MacroDefines_2_0=ruleWMLMacroDefine();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLRootRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"MacroDefines",
            	    	        		lv_MacroDefines_2_0, 
            	    	        		"WMLMacroDefine", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 4 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:167:6: ( (lv_Textdomains_3_0= ruleWMLTextdomain ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:167:6: ( (lv_Textdomains_3_0= ruleWMLTextdomain ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:168:1: (lv_Textdomains_3_0= ruleWMLTextdomain )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:168:1: (lv_Textdomains_3_0= ruleWMLTextdomain )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:169:3: lv_Textdomains_3_0= ruleWMLTextdomain
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLRootAccess().getTextdomainsWMLTextdomainParserRuleCall_3_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLTextdomain_in_ruleWMLRoot212);
            	    lv_Textdomains_3_0=ruleWMLTextdomain();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLRootRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Textdomains",
            	    	        		lv_Textdomains_3_0, 
            	    	        		"WMLTextdomain", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 5 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:192:6: ( (lv_IfDefs_4_0= ruleWMLPreprocIF ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:192:6: ( (lv_IfDefs_4_0= ruleWMLPreprocIF ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:193:1: (lv_IfDefs_4_0= ruleWMLPreprocIF )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:193:1: (lv_IfDefs_4_0= ruleWMLPreprocIF )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:194:3: lv_IfDefs_4_0= ruleWMLPreprocIF
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLRootAccess().getIfDefsWMLPreprocIFParserRuleCall_4_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLPreprocIF_in_ruleWMLRoot239);
            	    lv_IfDefs_4_0=ruleWMLPreprocIF();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLRootRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"IfDefs",
            	    	        		lv_IfDefs_4_0, 
            	    	        		"WMLPreprocIF", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;

            	default :
            	    break loop1;
                }
            } while (true);


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLRoot


    // $ANTLR start entryRuleWMLTag
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:224:1: entryRuleWMLTag returns [EObject current=null] : iv_ruleWMLTag= ruleWMLTag EOF ;
    public final EObject entryRuleWMLTag() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLTag = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:225:2: (iv_ruleWMLTag= ruleWMLTag EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:226:2: iv_ruleWMLTag= ruleWMLTag EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLTagRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLTag_in_entryRuleWMLTag276);
            iv_ruleWMLTag=ruleWMLTag();
            _fsp--;

             current =iv_ruleWMLTag; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLTag286); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLTag


    // $ANTLR start ruleWMLTag
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:233:1: ruleWMLTag returns [EObject current=null] : ( '[' ( (lv_plus_1_0= '+' ) )? ( (lv_name_2_0= RULE_ID ) ) ']' ( ( (lv_Tags_4_0= ruleWMLTag ) ) | ( (lv_Keys_5_0= ruleWMLKey ) ) | ( (lv_MacroCalls_6_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_7_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_8_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_9_0= ruleWMLPreprocIF ) ) )* '[/' ( (lv_endName_11_0= RULE_ID ) ) ']' ) ;
    public final EObject ruleWMLTag() throws RecognitionException {
        EObject current = null;

        Token lv_plus_1_0=null;
        Token lv_name_2_0=null;
        Token lv_endName_11_0=null;
        EObject lv_Tags_4_0 = null;

        EObject lv_Keys_5_0 = null;

        EObject lv_MacroCalls_6_0 = null;

        EObject lv_MacroDefines_7_0 = null;

        EObject lv_Textdomains_8_0 = null;

        EObject lv_IfDefs_9_0 = null;


         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:238:6: ( ( '[' ( (lv_plus_1_0= '+' ) )? ( (lv_name_2_0= RULE_ID ) ) ']' ( ( (lv_Tags_4_0= ruleWMLTag ) ) | ( (lv_Keys_5_0= ruleWMLKey ) ) | ( (lv_MacroCalls_6_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_7_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_8_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_9_0= ruleWMLPreprocIF ) ) )* '[/' ( (lv_endName_11_0= RULE_ID ) ) ']' ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:239:1: ( '[' ( (lv_plus_1_0= '+' ) )? ( (lv_name_2_0= RULE_ID ) ) ']' ( ( (lv_Tags_4_0= ruleWMLTag ) ) | ( (lv_Keys_5_0= ruleWMLKey ) ) | ( (lv_MacroCalls_6_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_7_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_8_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_9_0= ruleWMLPreprocIF ) ) )* '[/' ( (lv_endName_11_0= RULE_ID ) ) ']' )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:239:1: ( '[' ( (lv_plus_1_0= '+' ) )? ( (lv_name_2_0= RULE_ID ) ) ']' ( ( (lv_Tags_4_0= ruleWMLTag ) ) | ( (lv_Keys_5_0= ruleWMLKey ) ) | ( (lv_MacroCalls_6_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_7_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_8_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_9_0= ruleWMLPreprocIF ) ) )* '[/' ( (lv_endName_11_0= RULE_ID ) ) ']' )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:239:3: '[' ( (lv_plus_1_0= '+' ) )? ( (lv_name_2_0= RULE_ID ) ) ']' ( ( (lv_Tags_4_0= ruleWMLTag ) ) | ( (lv_Keys_5_0= ruleWMLKey ) ) | ( (lv_MacroCalls_6_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_7_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_8_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_9_0= ruleWMLPreprocIF ) ) )* '[/' ( (lv_endName_11_0= RULE_ID ) ) ']'
            {
            match(input,20,FOLLOW_20_in_ruleWMLTag321); 

                    createLeafNode(grammarAccess.getWMLTagAccess().getLeftSquareBracketKeyword_0(), null); 
                
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:243:1: ( (lv_plus_1_0= '+' ) )?
            int alt2=2;
            int LA2_0 = input.LA(1);

            if ( (LA2_0==21) ) {
                alt2=1;
            }
            switch (alt2) {
                case 1 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:244:1: (lv_plus_1_0= '+' )
                    {
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:244:1: (lv_plus_1_0= '+' )
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:245:3: lv_plus_1_0= '+'
                    {
                    lv_plus_1_0=(Token)input.LT(1);
                    match(input,21,FOLLOW_21_in_ruleWMLTag339); 

                            createLeafNode(grammarAccess.getWMLTagAccess().getPlusPlusSignKeyword_1_0(), "plus"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "plus", true, "+", lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }


                    }
                    break;

            }

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:264:3: ( (lv_name_2_0= RULE_ID ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:265:1: (lv_name_2_0= RULE_ID )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:265:1: (lv_name_2_0= RULE_ID )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:266:3: lv_name_2_0= RULE_ID
            {
            lv_name_2_0=(Token)input.LT(1);
            match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleWMLTag370); 

            			createLeafNode(grammarAccess.getWMLTagAccess().getNameIDTerminalRuleCall_2_0(), "name"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"name",
            	        		lv_name_2_0, 
            	        		"ID", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }

            match(input,22,FOLLOW_22_in_ruleWMLTag385); 

                    createLeafNode(grammarAccess.getWMLTagAccess().getRightSquareBracketKeyword_3(), null); 
                
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:292:1: ( ( (lv_Tags_4_0= ruleWMLTag ) ) | ( (lv_Keys_5_0= ruleWMLKey ) ) | ( (lv_MacroCalls_6_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_7_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_8_0= ruleWMLTextdomain ) ) | ( (lv_IfDefs_9_0= ruleWMLPreprocIF ) ) )*
            loop3:
            do {
                int alt3=7;
                switch ( input.LA(1) ) {
                case 20:
                    {
                    alt3=1;
                    }
                    break;
                case RULE_ID:
                    {
                    alt3=2;
                    }
                    break;
                case 25:
                    {
                    alt3=3;
                    }
                    break;
                case RULE_DEFINE:
                    {
                    alt3=4;
                    }
                    break;
                case RULE_TEXTDOMAIN:
                    {
                    alt3=5;
                    }
                    break;
                case RULE_IFDEF:
                case RULE_IFNDEF:
                case RULE_IFHAVE:
                case RULE_IFNHAVE:
                    {
                    alt3=6;
                    }
                    break;

                }

                switch (alt3) {
            	case 1 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:292:2: ( (lv_Tags_4_0= ruleWMLTag ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:292:2: ( (lv_Tags_4_0= ruleWMLTag ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:293:1: (lv_Tags_4_0= ruleWMLTag )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:293:1: (lv_Tags_4_0= ruleWMLTag )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:294:3: lv_Tags_4_0= ruleWMLTag
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLTagAccess().getTagsWMLTagParserRuleCall_4_0_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLTag_in_ruleWMLTag407);
            	    lv_Tags_4_0=ruleWMLTag();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Tags",
            	    	        		lv_Tags_4_0, 
            	    	        		"WMLTag", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 2 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:317:6: ( (lv_Keys_5_0= ruleWMLKey ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:317:6: ( (lv_Keys_5_0= ruleWMLKey ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:318:1: (lv_Keys_5_0= ruleWMLKey )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:318:1: (lv_Keys_5_0= ruleWMLKey )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:319:3: lv_Keys_5_0= ruleWMLKey
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLTagAccess().getKeysWMLKeyParserRuleCall_4_1_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLKey_in_ruleWMLTag434);
            	    lv_Keys_5_0=ruleWMLKey();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Keys",
            	    	        		lv_Keys_5_0, 
            	    	        		"WMLKey", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 3 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:342:6: ( (lv_MacroCalls_6_0= ruleWMLMacroCall ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:342:6: ( (lv_MacroCalls_6_0= ruleWMLMacroCall ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:343:1: (lv_MacroCalls_6_0= ruleWMLMacroCall )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:343:1: (lv_MacroCalls_6_0= ruleWMLMacroCall )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:344:3: lv_MacroCalls_6_0= ruleWMLMacroCall
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLTagAccess().getMacroCallsWMLMacroCallParserRuleCall_4_2_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroCall_in_ruleWMLTag461);
            	    lv_MacroCalls_6_0=ruleWMLMacroCall();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"MacroCalls",
            	    	        		lv_MacroCalls_6_0, 
            	    	        		"WMLMacroCall", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 4 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:367:6: ( (lv_MacroDefines_7_0= ruleWMLMacroDefine ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:367:6: ( (lv_MacroDefines_7_0= ruleWMLMacroDefine ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:368:1: (lv_MacroDefines_7_0= ruleWMLMacroDefine )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:368:1: (lv_MacroDefines_7_0= ruleWMLMacroDefine )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:369:3: lv_MacroDefines_7_0= ruleWMLMacroDefine
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLTagAccess().getMacroDefinesWMLMacroDefineParserRuleCall_4_3_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroDefine_in_ruleWMLTag488);
            	    lv_MacroDefines_7_0=ruleWMLMacroDefine();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"MacroDefines",
            	    	        		lv_MacroDefines_7_0, 
            	    	        		"WMLMacroDefine", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 5 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:392:6: ( (lv_Textdomains_8_0= ruleWMLTextdomain ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:392:6: ( (lv_Textdomains_8_0= ruleWMLTextdomain ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:393:1: (lv_Textdomains_8_0= ruleWMLTextdomain )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:393:1: (lv_Textdomains_8_0= ruleWMLTextdomain )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:394:3: lv_Textdomains_8_0= ruleWMLTextdomain
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLTagAccess().getTextdomainsWMLTextdomainParserRuleCall_4_4_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLTextdomain_in_ruleWMLTag515);
            	    lv_Textdomains_8_0=ruleWMLTextdomain();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Textdomains",
            	    	        		lv_Textdomains_8_0, 
            	    	        		"WMLTextdomain", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 6 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:417:6: ( (lv_IfDefs_9_0= ruleWMLPreprocIF ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:417:6: ( (lv_IfDefs_9_0= ruleWMLPreprocIF ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:418:1: (lv_IfDefs_9_0= ruleWMLPreprocIF )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:418:1: (lv_IfDefs_9_0= ruleWMLPreprocIF )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:419:3: lv_IfDefs_9_0= ruleWMLPreprocIF
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLTagAccess().getIfDefsWMLPreprocIFParserRuleCall_4_5_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLPreprocIF_in_ruleWMLTag542);
            	    lv_IfDefs_9_0=ruleWMLPreprocIF();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"IfDefs",
            	    	        		lv_IfDefs_9_0, 
            	    	        		"WMLPreprocIF", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;

            	default :
            	    break loop3;
                }
            } while (true);

            match(input,23,FOLLOW_23_in_ruleWMLTag554); 

                    createLeafNode(grammarAccess.getWMLTagAccess().getLeftSquareBracketSolidusKeyword_5(), null); 
                
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:445:1: ( (lv_endName_11_0= RULE_ID ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:446:1: (lv_endName_11_0= RULE_ID )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:446:1: (lv_endName_11_0= RULE_ID )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:447:3: lv_endName_11_0= RULE_ID
            {
            lv_endName_11_0=(Token)input.LT(1);
            match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleWMLTag571); 

            			createLeafNode(grammarAccess.getWMLTagAccess().getEndNameIDTerminalRuleCall_6_0(), "endName"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"endName",
            	        		lv_endName_11_0, 
            	        		"ID", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }

            match(input,22,FOLLOW_22_in_ruleWMLTag586); 

                    createLeafNode(grammarAccess.getWMLTagAccess().getRightSquareBracketKeyword_7(), null); 
                

            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLTag


    // $ANTLR start entryRuleWMLKey
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:481:1: entryRuleWMLKey returns [EObject current=null] : iv_ruleWMLKey= ruleWMLKey EOF ;
    public final EObject entryRuleWMLKey() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLKey = null;


         
        		HiddenTokens myHiddenTokenState = ((XtextTokenStream)input).setHiddenTokens("RULE_WS");
        	
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:485:2: (iv_ruleWMLKey= ruleWMLKey EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:486:2: iv_ruleWMLKey= ruleWMLKey EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLKeyRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLKey_in_entryRuleWMLKey628);
            iv_ruleWMLKey=ruleWMLKey();
            _fsp--;

             current =iv_ruleWMLKey; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLKey638); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {

            	myHiddenTokenState.restore();

        }
        return current;
    }
    // $ANTLR end entryRuleWMLKey


    // $ANTLR start ruleWMLKey
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:496:1: ruleWMLKey returns [EObject current=null] : ( ( (lv_name_0_0= RULE_ID ) ) '=' ( (lv_value_2_0= ruleWMLKeyValue ) )* ( ( RULE_EOL )? '+' ( RULE_EOL )? ( (lv_value_6_0= ruleWMLKeyValue ) )+ )* ( ( (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT ) ) ) ) ;
    public final EObject ruleWMLKey() throws RecognitionException {
        EObject current = null;

        Token lv_name_0_0=null;
        Token lv_eol_7_1=null;
        Token lv_eol_7_2=null;
        EObject lv_value_2_0 = null;

        EObject lv_value_6_0 = null;


         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
        		HiddenTokens myHiddenTokenState = ((XtextTokenStream)input).setHiddenTokens("RULE_WS");
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:502:6: ( ( ( (lv_name_0_0= RULE_ID ) ) '=' ( (lv_value_2_0= ruleWMLKeyValue ) )* ( ( RULE_EOL )? '+' ( RULE_EOL )? ( (lv_value_6_0= ruleWMLKeyValue ) )+ )* ( ( (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT ) ) ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:503:1: ( ( (lv_name_0_0= RULE_ID ) ) '=' ( (lv_value_2_0= ruleWMLKeyValue ) )* ( ( RULE_EOL )? '+' ( RULE_EOL )? ( (lv_value_6_0= ruleWMLKeyValue ) )+ )* ( ( (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT ) ) ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:503:1: ( ( (lv_name_0_0= RULE_ID ) ) '=' ( (lv_value_2_0= ruleWMLKeyValue ) )* ( ( RULE_EOL )? '+' ( RULE_EOL )? ( (lv_value_6_0= ruleWMLKeyValue ) )+ )* ( ( (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT ) ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:503:2: ( (lv_name_0_0= RULE_ID ) ) '=' ( (lv_value_2_0= ruleWMLKeyValue ) )* ( ( RULE_EOL )? '+' ( RULE_EOL )? ( (lv_value_6_0= ruleWMLKeyValue ) )+ )* ( ( (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT ) ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:503:2: ( (lv_name_0_0= RULE_ID ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:504:1: (lv_name_0_0= RULE_ID )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:504:1: (lv_name_0_0= RULE_ID )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:505:3: lv_name_0_0= RULE_ID
            {
            lv_name_0_0=(Token)input.LT(1);
            match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleWMLKey684); 

            			createLeafNode(grammarAccess.getWMLKeyAccess().getNameIDTerminalRuleCall_0_0(), "name"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"name",
            	        		lv_name_0_0, 
            	        		"ID", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }

            match(input,24,FOLLOW_24_in_ruleWMLKey699); 

                    createLeafNode(grammarAccess.getWMLKeyAccess().getEqualsSignKeyword_1(), null); 
                
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:531:1: ( (lv_value_2_0= ruleWMLKeyValue ) )*
            loop4:
            do {
                int alt4=2;
                int LA4_0 = input.LA(1);

                if ( (LA4_0==RULE_ID||LA4_0==RULE_LUA_CODE||(LA4_0>=RULE_STRING && LA4_0<=RULE_ANY_OTHER)||LA4_0==20||(LA4_0>=25 && LA4_0<=27)||(LA4_0>=29 && LA4_0<=34)) ) {
                    alt4=1;
                }


                switch (alt4) {
            	case 1 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:532:1: (lv_value_2_0= ruleWMLKeyValue )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:532:1: (lv_value_2_0= ruleWMLKeyValue )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:533:3: lv_value_2_0= ruleWMLKeyValue
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLKeyAccess().getValueWMLKeyValueParserRuleCall_2_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLKeyValue_in_ruleWMLKey720);
            	    lv_value_2_0=ruleWMLKeyValue();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"value",
            	    	        		lv_value_2_0, 
            	    	        		"WMLKeyValue", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }
            	    break;

            	default :
            	    break loop4;
                }
            } while (true);

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:555:3: ( ( RULE_EOL )? '+' ( RULE_EOL )? ( (lv_value_6_0= ruleWMLKeyValue ) )+ )*
            loop8:
            do {
                int alt8=2;
                int LA8_0 = input.LA(1);

                if ( (LA8_0==RULE_EOL) ) {
                    int LA8_1 = input.LA(2);

                    if ( (LA8_1==21) ) {
                        alt8=1;
                    }


                }
                else if ( (LA8_0==21) ) {
                    alt8=1;
                }


                switch (alt8) {
            	case 1 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:555:4: ( RULE_EOL )? '+' ( RULE_EOL )? ( (lv_value_6_0= ruleWMLKeyValue ) )+
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:555:4: ( RULE_EOL )?
            	    int alt5=2;
            	    int LA5_0 = input.LA(1);

            	    if ( (LA5_0==RULE_EOL) ) {
            	        alt5=1;
            	    }
            	    switch (alt5) {
            	        case 1 :
            	            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:555:5: RULE_EOL
            	            {
            	            match(input,RULE_EOL,FOLLOW_RULE_EOL_in_ruleWMLKey732); 
            	             
            	                createLeafNode(grammarAccess.getWMLKeyAccess().getEOLTerminalRuleCall_3_0(), null); 
            	                

            	            }
            	            break;

            	    }

            	    match(input,21,FOLLOW_21_in_ruleWMLKey743); 

            	            createLeafNode(grammarAccess.getWMLKeyAccess().getPlusSignKeyword_3_1(), null); 
            	        
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:563:1: ( RULE_EOL )?
            	    int alt6=2;
            	    int LA6_0 = input.LA(1);

            	    if ( (LA6_0==RULE_EOL) ) {
            	        alt6=1;
            	    }
            	    switch (alt6) {
            	        case 1 :
            	            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:563:2: RULE_EOL
            	            {
            	            match(input,RULE_EOL,FOLLOW_RULE_EOL_in_ruleWMLKey753); 
            	             
            	                createLeafNode(grammarAccess.getWMLKeyAccess().getEOLTerminalRuleCall_3_2(), null); 
            	                

            	            }
            	            break;

            	    }

            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:567:3: ( (lv_value_6_0= ruleWMLKeyValue ) )+
            	    int cnt7=0;
            	    loop7:
            	    do {
            	        int alt7=2;
            	        int LA7_0 = input.LA(1);

            	        if ( (LA7_0==RULE_ID||LA7_0==RULE_LUA_CODE||(LA7_0>=RULE_STRING && LA7_0<=RULE_ANY_OTHER)||LA7_0==20||(LA7_0>=25 && LA7_0<=27)||(LA7_0>=29 && LA7_0<=34)) ) {
            	            alt7=1;
            	        }


            	        switch (alt7) {
            	    	case 1 :
            	    	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:568:1: (lv_value_6_0= ruleWMLKeyValue )
            	    	    {
            	    	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:568:1: (lv_value_6_0= ruleWMLKeyValue )
            	    	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:569:3: lv_value_6_0= ruleWMLKeyValue
            	    	    {
            	    	     
            	    	    	        currentNode=createCompositeNode(grammarAccess.getWMLKeyAccess().getValueWMLKeyValueParserRuleCall_3_3_0(), currentNode); 
            	    	    	    
            	    	    pushFollow(FOLLOW_ruleWMLKeyValue_in_ruleWMLKey775);
            	    	    lv_value_6_0=ruleWMLKeyValue();
            	    	    _fsp--;


            	    	    	        if (current==null) {
            	    	    	            current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
            	    	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	    	        }
            	    	    	        try {
            	    	    	       		add(
            	    	    	       			current, 
            	    	    	       			"value",
            	    	    	        		lv_value_6_0, 
            	    	    	        		"WMLKeyValue", 
            	    	    	        		currentNode);
            	    	    	        } catch (ValueConverterException vce) {
            	    	    				handleValueConverterException(vce);
            	    	    	        }
            	    	    	        currentNode = currentNode.getParent();
            	    	    	    

            	    	    }


            	    	    }
            	    	    break;

            	    	default :
            	    	    if ( cnt7 >= 1 ) break loop7;
            	                EarlyExitException eee =
            	                    new EarlyExitException(7, input);
            	                throw eee;
            	        }
            	        cnt7++;
            	    } while (true);


            	    }
            	    break;

            	default :
            	    break loop8;
                }
            } while (true);

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:591:5: ( ( (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:592:1: ( (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:592:1: ( (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:593:1: (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:593:1: (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT )
            int alt9=2;
            int LA9_0 = input.LA(1);

            if ( (LA9_0==RULE_EOL) ) {
                alt9=1;
            }
            else if ( (LA9_0==RULE_SL_COMMENT) ) {
                alt9=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("593:1: (lv_eol_7_1= RULE_EOL | lv_eol_7_2= RULE_SL_COMMENT )", 9, 0, input);

                throw nvae;
            }
            switch (alt9) {
                case 1 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:594:3: lv_eol_7_1= RULE_EOL
                    {
                    lv_eol_7_1=(Token)input.LT(1);
                    match(input,RULE_EOL,FOLLOW_RULE_EOL_in_ruleWMLKey797); 

                    			createLeafNode(grammarAccess.getWMLKeyAccess().getEolEOLTerminalRuleCall_4_0_0(), "eol"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"eol",
                    	        		lv_eol_7_1, 
                    	        		"EOL", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 2 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:615:8: lv_eol_7_2= RULE_SL_COMMENT
                    {
                    lv_eol_7_2=(Token)input.LT(1);
                    match(input,RULE_SL_COMMENT,FOLLOW_RULE_SL_COMMENT_in_ruleWMLKey817); 

                    			createLeafNode(grammarAccess.getWMLKeyAccess().getEolSL_COMMENTTerminalRuleCall_4_0_1(), "eol"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"eol",
                    	        		lv_eol_7_2, 
                    	        		"SL_COMMENT", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;

            }


            }


            }


            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {

            	myHiddenTokenState.restore();

        }
        return current;
    }
    // $ANTLR end ruleWMLKey


    // $ANTLR start entryRuleWMLKeyValue
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:650:1: entryRuleWMLKeyValue returns [EObject current=null] : iv_ruleWMLKeyValue= ruleWMLKeyValue EOF ;
    public final EObject entryRuleWMLKeyValue() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLKeyValue = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:651:2: (iv_ruleWMLKeyValue= ruleWMLKeyValue EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:652:2: iv_ruleWMLKeyValue= ruleWMLKeyValue EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLKeyValueRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLKeyValue_in_entryRuleWMLKeyValue865);
            iv_ruleWMLKeyValue=ruleWMLKeyValue();
            _fsp--;

             current =iv_ruleWMLKeyValue; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLKeyValue875); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLKeyValue


    // $ANTLR start ruleWMLKeyValue
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:659:1: ruleWMLKeyValue returns [EObject current=null] : (this_WMLValue_0= ruleWMLValue | this_WMLMacroCall_1= ruleWMLMacroCall | this_WMLLuaCode_2= ruleWMLLuaCode | this_WMLArrayCall_3= ruleWMLArrayCall ) ;
    public final EObject ruleWMLKeyValue() throws RecognitionException {
        EObject current = null;

        EObject this_WMLValue_0 = null;

        EObject this_WMLMacroCall_1 = null;

        EObject this_WMLLuaCode_2 = null;

        EObject this_WMLArrayCall_3 = null;


         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:664:6: ( (this_WMLValue_0= ruleWMLValue | this_WMLMacroCall_1= ruleWMLMacroCall | this_WMLLuaCode_2= ruleWMLLuaCode | this_WMLArrayCall_3= ruleWMLArrayCall ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:665:1: (this_WMLValue_0= ruleWMLValue | this_WMLMacroCall_1= ruleWMLMacroCall | this_WMLLuaCode_2= ruleWMLLuaCode | this_WMLArrayCall_3= ruleWMLArrayCall )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:665:1: (this_WMLValue_0= ruleWMLValue | this_WMLMacroCall_1= ruleWMLMacroCall | this_WMLLuaCode_2= ruleWMLLuaCode | this_WMLArrayCall_3= ruleWMLArrayCall )
            int alt10=4;
            switch ( input.LA(1) ) {
            case RULE_ID:
            case RULE_STRING:
            case RULE_ANY_OTHER:
            case 26:
            case 27:
            case 29:
            case 30:
            case 31:
            case 32:
            case 33:
            case 34:
                {
                alt10=1;
                }
                break;
            case 25:
                {
                alt10=2;
                }
                break;
            case RULE_LUA_CODE:
                {
                alt10=3;
                }
                break;
            case 20:
                {
                alt10=4;
                }
                break;
            default:
                NoViableAltException nvae =
                    new NoViableAltException("665:1: (this_WMLValue_0= ruleWMLValue | this_WMLMacroCall_1= ruleWMLMacroCall | this_WMLLuaCode_2= ruleWMLLuaCode | this_WMLArrayCall_3= ruleWMLArrayCall )", 10, 0, input);

                throw nvae;
            }

            switch (alt10) {
                case 1 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:666:5: this_WMLValue_0= ruleWMLValue
                    {
                     
                            currentNode=createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLValueParserRuleCall_0(), currentNode); 
                        
                    pushFollow(FOLLOW_ruleWMLValue_in_ruleWMLKeyValue922);
                    this_WMLValue_0=ruleWMLValue();
                    _fsp--;

                     
                            current = this_WMLValue_0; 
                            currentNode = currentNode.getParent();
                        

                    }
                    break;
                case 2 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:676:5: this_WMLMacroCall_1= ruleWMLMacroCall
                    {
                     
                            currentNode=createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLMacroCallParserRuleCall_1(), currentNode); 
                        
                    pushFollow(FOLLOW_ruleWMLMacroCall_in_ruleWMLKeyValue949);
                    this_WMLMacroCall_1=ruleWMLMacroCall();
                    _fsp--;

                     
                            current = this_WMLMacroCall_1; 
                            currentNode = currentNode.getParent();
                        

                    }
                    break;
                case 3 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:686:5: this_WMLLuaCode_2= ruleWMLLuaCode
                    {
                     
                            currentNode=createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLLuaCodeParserRuleCall_2(), currentNode); 
                        
                    pushFollow(FOLLOW_ruleWMLLuaCode_in_ruleWMLKeyValue976);
                    this_WMLLuaCode_2=ruleWMLLuaCode();
                    _fsp--;

                     
                            current = this_WMLLuaCode_2; 
                            currentNode = currentNode.getParent();
                        

                    }
                    break;
                case 4 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:696:5: this_WMLArrayCall_3= ruleWMLArrayCall
                    {
                     
                            currentNode=createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLArrayCallParserRuleCall_3(), currentNode); 
                        
                    pushFollow(FOLLOW_ruleWMLArrayCall_in_ruleWMLKeyValue1003);
                    this_WMLArrayCall_3=ruleWMLArrayCall();
                    _fsp--;

                     
                            current = this_WMLArrayCall_3; 
                            currentNode = currentNode.getParent();
                        

                    }
                    break;

            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLKeyValue


    // $ANTLR start entryRuleWMLMacroCall
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:712:1: entryRuleWMLMacroCall returns [EObject current=null] : iv_ruleWMLMacroCall= ruleWMLMacroCall EOF ;
    public final EObject entryRuleWMLMacroCall() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLMacroCall = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:713:2: (iv_ruleWMLMacroCall= ruleWMLMacroCall EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:714:2: iv_ruleWMLMacroCall= ruleWMLMacroCall EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLMacroCallRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLMacroCall_in_entryRuleWMLMacroCall1038);
            iv_ruleWMLMacroCall=ruleWMLMacroCall();
            _fsp--;

             current =iv_ruleWMLMacroCall; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLMacroCall1048); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLMacroCall


    // $ANTLR start ruleWMLMacroCall
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:721:1: ruleWMLMacroCall returns [EObject current=null] : ( '{' ( (lv_point_1_0= './' ) )? ( (lv_relative_2_0= '~' ) )? ( (lv_name_3_0= RULE_ID ) ) ( ( ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) ) ) | ( (lv_extraMacros_5_0= ruleWMLMacroCall ) ) )* '}' ) ;
    public final EObject ruleWMLMacroCall() throws RecognitionException {
        EObject current = null;

        Token lv_point_1_0=null;
        Token lv_relative_2_0=null;
        Token lv_name_3_0=null;
        EObject lv_params_4_1 = null;

        EObject lv_params_4_2 = null;

        EObject lv_extraMacros_5_0 = null;


         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:726:6: ( ( '{' ( (lv_point_1_0= './' ) )? ( (lv_relative_2_0= '~' ) )? ( (lv_name_3_0= RULE_ID ) ) ( ( ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) ) ) | ( (lv_extraMacros_5_0= ruleWMLMacroCall ) ) )* '}' ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:727:1: ( '{' ( (lv_point_1_0= './' ) )? ( (lv_relative_2_0= '~' ) )? ( (lv_name_3_0= RULE_ID ) ) ( ( ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) ) ) | ( (lv_extraMacros_5_0= ruleWMLMacroCall ) ) )* '}' )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:727:1: ( '{' ( (lv_point_1_0= './' ) )? ( (lv_relative_2_0= '~' ) )? ( (lv_name_3_0= RULE_ID ) ) ( ( ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) ) ) | ( (lv_extraMacros_5_0= ruleWMLMacroCall ) ) )* '}' )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:727:3: '{' ( (lv_point_1_0= './' ) )? ( (lv_relative_2_0= '~' ) )? ( (lv_name_3_0= RULE_ID ) ) ( ( ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) ) ) | ( (lv_extraMacros_5_0= ruleWMLMacroCall ) ) )* '}'
            {
            match(input,25,FOLLOW_25_in_ruleWMLMacroCall1083); 

                    createLeafNode(grammarAccess.getWMLMacroCallAccess().getLeftCurlyBracketKeyword_0(), null); 
                
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:731:1: ( (lv_point_1_0= './' ) )?
            int alt11=2;
            int LA11_0 = input.LA(1);

            if ( (LA11_0==26) ) {
                alt11=1;
            }
            switch (alt11) {
                case 1 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:732:1: (lv_point_1_0= './' )
                    {
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:732:1: (lv_point_1_0= './' )
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:733:3: lv_point_1_0= './'
                    {
                    lv_point_1_0=(Token)input.LT(1);
                    match(input,26,FOLLOW_26_in_ruleWMLMacroCall1101); 

                            createLeafNode(grammarAccess.getWMLMacroCallAccess().getPointFullStopSolidusKeyword_1_0(), "point"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "point", true, "./", lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }


                    }
                    break;

            }

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:752:3: ( (lv_relative_2_0= '~' ) )?
            int alt12=2;
            int LA12_0 = input.LA(1);

            if ( (LA12_0==27) ) {
                alt12=1;
            }
            switch (alt12) {
                case 1 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:753:1: (lv_relative_2_0= '~' )
                    {
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:753:1: (lv_relative_2_0= '~' )
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:754:3: lv_relative_2_0= '~'
                    {
                    lv_relative_2_0=(Token)input.LT(1);
                    match(input,27,FOLLOW_27_in_ruleWMLMacroCall1133); 

                            createLeafNode(grammarAccess.getWMLMacroCallAccess().getRelativeTildeKeyword_2_0(), "relative"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "relative", true, "~", lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }


                    }
                    break;

            }

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:773:3: ( (lv_name_3_0= RULE_ID ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:774:1: (lv_name_3_0= RULE_ID )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:774:1: (lv_name_3_0= RULE_ID )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:775:3: lv_name_3_0= RULE_ID
            {
            lv_name_3_0=(Token)input.LT(1);
            match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleWMLMacroCall1164); 

            			createLeafNode(grammarAccess.getWMLMacroCallAccess().getNameIDTerminalRuleCall_3_0(), "name"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"name",
            	        		lv_name_3_0, 
            	        		"ID", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:797:2: ( ( ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) ) ) | ( (lv_extraMacros_5_0= ruleWMLMacroCall ) ) )*
            loop14:
            do {
                int alt14=3;
                int LA14_0 = input.LA(1);

                if ( (LA14_0==RULE_ID||(LA14_0>=RULE_STRING && LA14_0<=RULE_ANY_OTHER)||(LA14_0>=20 && LA14_0<=24)||(LA14_0>=26 && LA14_0<=27)||(LA14_0>=29 && LA14_0<=34)) ) {
                    alt14=1;
                }
                else if ( (LA14_0==25) ) {
                    alt14=2;
                }


                switch (alt14) {
            	case 1 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:797:3: ( ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:797:3: ( ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:798:1: ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:798:1: ( (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:799:1: (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:799:1: (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens )
            	    int alt13=2;
            	    int LA13_0 = input.LA(1);

            	    if ( (LA13_0==RULE_ID||(LA13_0>=RULE_STRING && LA13_0<=RULE_ANY_OTHER)||(LA13_0>=26 && LA13_0<=27)||(LA13_0>=29 && LA13_0<=34)) ) {
            	        alt13=1;
            	    }
            	    else if ( ((LA13_0>=20 && LA13_0<=24)) ) {
            	        alt13=2;
            	    }
            	    else {
            	        NoViableAltException nvae =
            	            new NoViableAltException("799:1: (lv_params_4_1= ruleWMLValue | lv_params_4_2= ruleMacroTokens )", 13, 0, input);

            	        throw nvae;
            	    }
            	    switch (alt13) {
            	        case 1 :
            	            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:800:3: lv_params_4_1= ruleWMLValue
            	            {
            	             
            	            	        currentNode=createCompositeNode(grammarAccess.getWMLMacroCallAccess().getParamsWMLValueParserRuleCall_4_0_0_0(), currentNode); 
            	            	    
            	            pushFollow(FOLLOW_ruleWMLValue_in_ruleWMLMacroCall1193);
            	            lv_params_4_1=ruleWMLValue();
            	            _fsp--;


            	            	        if (current==null) {
            	            	            current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
            	            	            associateNodeWithAstElement(currentNode.getParent(), current);
            	            	        }
            	            	        try {
            	            	       		add(
            	            	       			current, 
            	            	       			"params",
            	            	        		lv_params_4_1, 
            	            	        		"WMLValue", 
            	            	        		currentNode);
            	            	        } catch (ValueConverterException vce) {
            	            				handleValueConverterException(vce);
            	            	        }
            	            	        currentNode = currentNode.getParent();
            	            	    

            	            }
            	            break;
            	        case 2 :
            	            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:821:8: lv_params_4_2= ruleMacroTokens
            	            {
            	             
            	            	        currentNode=createCompositeNode(grammarAccess.getWMLMacroCallAccess().getParamsMacroTokensParserRuleCall_4_0_0_1(), currentNode); 
            	            	    
            	            pushFollow(FOLLOW_ruleMacroTokens_in_ruleWMLMacroCall1212);
            	            lv_params_4_2=ruleMacroTokens();
            	            _fsp--;


            	            	        if (current==null) {
            	            	            current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
            	            	            associateNodeWithAstElement(currentNode.getParent(), current);
            	            	        }
            	            	        try {
            	            	       		add(
            	            	       			current, 
            	            	       			"params",
            	            	        		lv_params_4_2, 
            	            	        		"MacroTokens", 
            	            	        		currentNode);
            	            	        } catch (ValueConverterException vce) {
            	            				handleValueConverterException(vce);
            	            	        }
            	            	        currentNode = currentNode.getParent();
            	            	    

            	            }
            	            break;

            	    }


            	    }


            	    }


            	    }
            	    break;
            	case 2 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:846:6: ( (lv_extraMacros_5_0= ruleWMLMacroCall ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:846:6: ( (lv_extraMacros_5_0= ruleWMLMacroCall ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:847:1: (lv_extraMacros_5_0= ruleWMLMacroCall )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:847:1: (lv_extraMacros_5_0= ruleWMLMacroCall )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:848:3: lv_extraMacros_5_0= ruleWMLMacroCall
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLMacroCallAccess().getExtraMacrosWMLMacroCallParserRuleCall_4_1_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroCall_in_ruleWMLMacroCall1242);
            	    lv_extraMacros_5_0=ruleWMLMacroCall();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"extraMacros",
            	    	        		lv_extraMacros_5_0, 
            	    	        		"WMLMacroCall", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;

            	default :
            	    break loop14;
                }
            } while (true);

            match(input,28,FOLLOW_28_in_ruleWMLMacroCall1254); 

                    createLeafNode(grammarAccess.getWMLMacroCallAccess().getRightCurlyBracketKeyword_5(), null); 
                

            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLMacroCall


    // $ANTLR start entryRuleWMLLuaCode
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:882:1: entryRuleWMLLuaCode returns [EObject current=null] : iv_ruleWMLLuaCode= ruleWMLLuaCode EOF ;
    public final EObject entryRuleWMLLuaCode() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLLuaCode = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:883:2: (iv_ruleWMLLuaCode= ruleWMLLuaCode EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:884:2: iv_ruleWMLLuaCode= ruleWMLLuaCode EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLLuaCodeRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLLuaCode_in_entryRuleWMLLuaCode1290);
            iv_ruleWMLLuaCode=ruleWMLLuaCode();
            _fsp--;

             current =iv_ruleWMLLuaCode; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLLuaCode1300); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLLuaCode


    // $ANTLR start ruleWMLLuaCode
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:891:1: ruleWMLLuaCode returns [EObject current=null] : ( (lv_value_0_0= RULE_LUA_CODE ) ) ;
    public final EObject ruleWMLLuaCode() throws RecognitionException {
        EObject current = null;

        Token lv_value_0_0=null;

         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:896:6: ( ( (lv_value_0_0= RULE_LUA_CODE ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:897:1: ( (lv_value_0_0= RULE_LUA_CODE ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:897:1: ( (lv_value_0_0= RULE_LUA_CODE ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:898:1: (lv_value_0_0= RULE_LUA_CODE )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:898:1: (lv_value_0_0= RULE_LUA_CODE )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:899:3: lv_value_0_0= RULE_LUA_CODE
            {
            lv_value_0_0=(Token)input.LT(1);
            match(input,RULE_LUA_CODE,FOLLOW_RULE_LUA_CODE_in_ruleWMLLuaCode1341); 

            			createLeafNode(grammarAccess.getWMLLuaCodeAccess().getValueLUA_CODETerminalRuleCall_0(), "value"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLLuaCodeRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"value",
            	        		lv_value_0_0, 
            	        		"LUA_CODE", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLLuaCode


    // $ANTLR start entryRuleWMLArrayCall
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:929:1: entryRuleWMLArrayCall returns [EObject current=null] : iv_ruleWMLArrayCall= ruleWMLArrayCall EOF ;
    public final EObject entryRuleWMLArrayCall() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLArrayCall = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:930:2: (iv_ruleWMLArrayCall= ruleWMLArrayCall EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:931:2: iv_ruleWMLArrayCall= ruleWMLArrayCall EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLArrayCallRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLArrayCall_in_entryRuleWMLArrayCall1381);
            iv_ruleWMLArrayCall=ruleWMLArrayCall();
            _fsp--;

             current =iv_ruleWMLArrayCall; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLArrayCall1391); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLArrayCall


    // $ANTLR start ruleWMLArrayCall
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:938:1: ruleWMLArrayCall returns [EObject current=null] : ( '[' ( (lv_value_1_0= ruleWMLValue ) )+ ']' ) ;
    public final EObject ruleWMLArrayCall() throws RecognitionException {
        EObject current = null;

        EObject lv_value_1_0 = null;


         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:943:6: ( ( '[' ( (lv_value_1_0= ruleWMLValue ) )+ ']' ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:944:1: ( '[' ( (lv_value_1_0= ruleWMLValue ) )+ ']' )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:944:1: ( '[' ( (lv_value_1_0= ruleWMLValue ) )+ ']' )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:944:3: '[' ( (lv_value_1_0= ruleWMLValue ) )+ ']'
            {
            match(input,20,FOLLOW_20_in_ruleWMLArrayCall1426); 

                    createLeafNode(grammarAccess.getWMLArrayCallAccess().getLeftSquareBracketKeyword_0(), null); 
                
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:948:1: ( (lv_value_1_0= ruleWMLValue ) )+
            int cnt15=0;
            loop15:
            do {
                int alt15=2;
                int LA15_0 = input.LA(1);

                if ( (LA15_0==RULE_ID||(LA15_0>=RULE_STRING && LA15_0<=RULE_ANY_OTHER)||(LA15_0>=26 && LA15_0<=27)||(LA15_0>=29 && LA15_0<=34)) ) {
                    alt15=1;
                }


                switch (alt15) {
            	case 1 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:949:1: (lv_value_1_0= ruleWMLValue )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:949:1: (lv_value_1_0= ruleWMLValue )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:950:3: lv_value_1_0= ruleWMLValue
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLArrayCallAccess().getValueWMLValueParserRuleCall_1_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLValue_in_ruleWMLArrayCall1447);
            	    lv_value_1_0=ruleWMLValue();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLArrayCallRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"value",
            	    	        		lv_value_1_0, 
            	    	        		"WMLValue", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }
            	    break;

            	default :
            	    if ( cnt15 >= 1 ) break loop15;
                        EarlyExitException eee =
                            new EarlyExitException(15, input);
                        throw eee;
                }
                cnt15++;
            } while (true);

            match(input,22,FOLLOW_22_in_ruleWMLArrayCall1458); 

                    createLeafNode(grammarAccess.getWMLArrayCallAccess().getRightSquareBracketKeyword_2(), null); 
                

            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLArrayCall


    // $ANTLR start entryRuleWMLMacroDefine
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:984:1: entryRuleWMLMacroDefine returns [EObject current=null] : iv_ruleWMLMacroDefine= ruleWMLMacroDefine EOF ;
    public final EObject entryRuleWMLMacroDefine() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLMacroDefine = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:985:2: (iv_ruleWMLMacroDefine= ruleWMLMacroDefine EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:986:2: iv_ruleWMLMacroDefine= ruleWMLMacroDefine EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLMacroDefineRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLMacroDefine_in_entryRuleWMLMacroDefine1494);
            iv_ruleWMLMacroDefine=ruleWMLMacroDefine();
            _fsp--;

             current =iv_ruleWMLMacroDefine; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLMacroDefine1504); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLMacroDefine


    // $ANTLR start ruleWMLMacroDefine
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:993:1: ruleWMLMacroDefine returns [EObject current=null] : ( ( (lv_name_0_0= RULE_DEFINE ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) )* ( (lv_endName_8_0= RULE_ENDDEF ) ) ) ;
    public final EObject ruleWMLMacroDefine() throws RecognitionException {
        EObject current = null;

        Token lv_name_0_0=null;
        Token lv_endName_8_0=null;
        EObject lv_Tags_1_0 = null;

        EObject lv_Keys_2_0 = null;

        EObject lv_MacroCalls_3_0 = null;

        EObject lv_MacroDefines_4_0 = null;

        EObject lv_Textdomains_5_0 = null;

        EObject lv_Values_6_0 = null;

        EObject lv_IfDefs_7_0 = null;


         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:998:6: ( ( ( (lv_name_0_0= RULE_DEFINE ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) )* ( (lv_endName_8_0= RULE_ENDDEF ) ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:999:1: ( ( (lv_name_0_0= RULE_DEFINE ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) )* ( (lv_endName_8_0= RULE_ENDDEF ) ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:999:1: ( ( (lv_name_0_0= RULE_DEFINE ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) )* ( (lv_endName_8_0= RULE_ENDDEF ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:999:2: ( (lv_name_0_0= RULE_DEFINE ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) )* ( (lv_endName_8_0= RULE_ENDDEF ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:999:2: ( (lv_name_0_0= RULE_DEFINE ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1000:1: (lv_name_0_0= RULE_DEFINE )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1000:1: (lv_name_0_0= RULE_DEFINE )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1001:3: lv_name_0_0= RULE_DEFINE
            {
            lv_name_0_0=(Token)input.LT(1);
            match(input,RULE_DEFINE,FOLLOW_RULE_DEFINE_in_ruleWMLMacroDefine1546); 

            			createLeafNode(grammarAccess.getWMLMacroDefineAccess().getNameDEFINETerminalRuleCall_0_0(), "name"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"name",
            	        		lv_name_0_0, 
            	        		"DEFINE", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1023:2: ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) )*
            loop16:
            do {
                int alt16=8;
                switch ( input.LA(1) ) {
                case 20:
                    {
                    alt16=1;
                    }
                    break;
                case RULE_ID:
                    {
                    int LA16_3 = input.LA(2);

                    if ( (LA16_3==24) ) {
                        alt16=2;
                    }
                    else if ( (LA16_3==RULE_ID||(LA16_3>=RULE_DEFINE && LA16_3<=RULE_IFNHAVE)||(LA16_3>=RULE_TEXTDOMAIN && LA16_3<=RULE_ANY_OTHER)||LA16_3==20||(LA16_3>=25 && LA16_3<=27)||(LA16_3>=29 && LA16_3<=34)) ) {
                        alt16=6;
                    }


                    }
                    break;
                case 25:
                    {
                    alt16=3;
                    }
                    break;
                case RULE_DEFINE:
                    {
                    alt16=4;
                    }
                    break;
                case RULE_TEXTDOMAIN:
                    {
                    alt16=5;
                    }
                    break;
                case RULE_STRING:
                case RULE_ANY_OTHER:
                case 26:
                case 27:
                case 29:
                case 30:
                case 31:
                case 32:
                case 33:
                case 34:
                    {
                    alt16=6;
                    }
                    break;
                case RULE_IFDEF:
                case RULE_IFNDEF:
                case RULE_IFHAVE:
                case RULE_IFNHAVE:
                    {
                    alt16=7;
                    }
                    break;

                }

                switch (alt16) {
            	case 1 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1023:3: ( (lv_Tags_1_0= ruleWMLTag ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1023:3: ( (lv_Tags_1_0= ruleWMLTag ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1024:1: (lv_Tags_1_0= ruleWMLTag )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1024:1: (lv_Tags_1_0= ruleWMLTag )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1025:3: lv_Tags_1_0= ruleWMLTag
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLMacroDefineAccess().getTagsWMLTagParserRuleCall_1_0_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLTag_in_ruleWMLMacroDefine1573);
            	    lv_Tags_1_0=ruleWMLTag();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Tags",
            	    	        		lv_Tags_1_0, 
            	    	        		"WMLTag", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 2 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1048:6: ( (lv_Keys_2_0= ruleWMLKey ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1048:6: ( (lv_Keys_2_0= ruleWMLKey ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1049:1: (lv_Keys_2_0= ruleWMLKey )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1049:1: (lv_Keys_2_0= ruleWMLKey )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1050:3: lv_Keys_2_0= ruleWMLKey
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLMacroDefineAccess().getKeysWMLKeyParserRuleCall_1_1_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLKey_in_ruleWMLMacroDefine1600);
            	    lv_Keys_2_0=ruleWMLKey();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Keys",
            	    	        		lv_Keys_2_0, 
            	    	        		"WMLKey", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 3 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1073:6: ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1073:6: ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1074:1: (lv_MacroCalls_3_0= ruleWMLMacroCall )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1074:1: (lv_MacroCalls_3_0= ruleWMLMacroCall )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1075:3: lv_MacroCalls_3_0= ruleWMLMacroCall
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLMacroDefineAccess().getMacroCallsWMLMacroCallParserRuleCall_1_2_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroCall_in_ruleWMLMacroDefine1627);
            	    lv_MacroCalls_3_0=ruleWMLMacroCall();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"MacroCalls",
            	    	        		lv_MacroCalls_3_0, 
            	    	        		"WMLMacroCall", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 4 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1098:6: ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1098:6: ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1099:1: (lv_MacroDefines_4_0= ruleWMLMacroDefine )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1099:1: (lv_MacroDefines_4_0= ruleWMLMacroDefine )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1100:3: lv_MacroDefines_4_0= ruleWMLMacroDefine
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLMacroDefineAccess().getMacroDefinesWMLMacroDefineParserRuleCall_1_3_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroDefine_in_ruleWMLMacroDefine1654);
            	    lv_MacroDefines_4_0=ruleWMLMacroDefine();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"MacroDefines",
            	    	        		lv_MacroDefines_4_0, 
            	    	        		"WMLMacroDefine", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 5 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1123:6: ( (lv_Textdomains_5_0= ruleWMLTextdomain ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1123:6: ( (lv_Textdomains_5_0= ruleWMLTextdomain ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1124:1: (lv_Textdomains_5_0= ruleWMLTextdomain )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1124:1: (lv_Textdomains_5_0= ruleWMLTextdomain )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1125:3: lv_Textdomains_5_0= ruleWMLTextdomain
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLMacroDefineAccess().getTextdomainsWMLTextdomainParserRuleCall_1_4_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLTextdomain_in_ruleWMLMacroDefine1681);
            	    lv_Textdomains_5_0=ruleWMLTextdomain();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Textdomains",
            	    	        		lv_Textdomains_5_0, 
            	    	        		"WMLTextdomain", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 6 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1148:6: ( (lv_Values_6_0= ruleWMLValue ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1148:6: ( (lv_Values_6_0= ruleWMLValue ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1149:1: (lv_Values_6_0= ruleWMLValue )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1149:1: (lv_Values_6_0= ruleWMLValue )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1150:3: lv_Values_6_0= ruleWMLValue
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLMacroDefineAccess().getValuesWMLValueParserRuleCall_1_5_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLValue_in_ruleWMLMacroDefine1708);
            	    lv_Values_6_0=ruleWMLValue();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Values",
            	    	        		lv_Values_6_0, 
            	    	        		"WMLValue", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 7 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1173:6: ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1173:6: ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1174:1: (lv_IfDefs_7_0= ruleWMLPreprocIF )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1174:1: (lv_IfDefs_7_0= ruleWMLPreprocIF )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1175:3: lv_IfDefs_7_0= ruleWMLPreprocIF
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLMacroDefineAccess().getIfDefsWMLPreprocIFParserRuleCall_1_6_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLPreprocIF_in_ruleWMLMacroDefine1735);
            	    lv_IfDefs_7_0=ruleWMLPreprocIF();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"IfDefs",
            	    	        		lv_IfDefs_7_0, 
            	    	        		"WMLPreprocIF", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;

            	default :
            	    break loop16;
                }
            } while (true);

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1197:4: ( (lv_endName_8_0= RULE_ENDDEF ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1198:1: (lv_endName_8_0= RULE_ENDDEF )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1198:1: (lv_endName_8_0= RULE_ENDDEF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1199:3: lv_endName_8_0= RULE_ENDDEF
            {
            lv_endName_8_0=(Token)input.LT(1);
            match(input,RULE_ENDDEF,FOLLOW_RULE_ENDDEF_in_ruleWMLMacroDefine1754); 

            			createLeafNode(grammarAccess.getWMLMacroDefineAccess().getEndNameENDDEFTerminalRuleCall_2_0(), "endName"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"endName",
            	        		lv_endName_8_0, 
            	        		"ENDDEF", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }


            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLMacroDefine


    // $ANTLR start entryRuleWMLPreprocIF
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1229:1: entryRuleWMLPreprocIF returns [EObject current=null] : iv_ruleWMLPreprocIF= ruleWMLPreprocIF EOF ;
    public final EObject entryRuleWMLPreprocIF() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLPreprocIF = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1230:2: (iv_ruleWMLPreprocIF= ruleWMLPreprocIF EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1231:2: iv_ruleWMLPreprocIF= ruleWMLPreprocIF EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLPreprocIFRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLPreprocIF_in_entryRuleWMLPreprocIF1795);
            iv_ruleWMLPreprocIF=ruleWMLPreprocIF();
            _fsp--;

             current =iv_ruleWMLPreprocIF; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLPreprocIF1805); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLPreprocIF


    // $ANTLR start ruleWMLPreprocIF
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1238:1: ruleWMLPreprocIF returns [EObject current=null] : ( ( ( (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE ) ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) | ( (lv_Elses_8_0= RULE_ELSE ) ) )* ( (lv_endName_9_0= RULE_ENDIF ) ) ) ;
    public final EObject ruleWMLPreprocIF() throws RecognitionException {
        EObject current = null;

        Token lv_name_0_1=null;
        Token lv_name_0_2=null;
        Token lv_name_0_3=null;
        Token lv_name_0_4=null;
        Token lv_Elses_8_0=null;
        Token lv_endName_9_0=null;
        EObject lv_Tags_1_0 = null;

        EObject lv_Keys_2_0 = null;

        EObject lv_MacroCalls_3_0 = null;

        EObject lv_MacroDefines_4_0 = null;

        EObject lv_Textdomains_5_0 = null;

        EObject lv_Values_6_0 = null;

        EObject lv_IfDefs_7_0 = null;


         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1243:6: ( ( ( ( (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE ) ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) | ( (lv_Elses_8_0= RULE_ELSE ) ) )* ( (lv_endName_9_0= RULE_ENDIF ) ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1244:1: ( ( ( (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE ) ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) | ( (lv_Elses_8_0= RULE_ELSE ) ) )* ( (lv_endName_9_0= RULE_ENDIF ) ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1244:1: ( ( ( (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE ) ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) | ( (lv_Elses_8_0= RULE_ELSE ) ) )* ( (lv_endName_9_0= RULE_ENDIF ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1244:2: ( ( (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE ) ) ) ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) | ( (lv_Elses_8_0= RULE_ELSE ) ) )* ( (lv_endName_9_0= RULE_ENDIF ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1244:2: ( ( (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1245:1: ( (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1245:1: ( (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1246:1: (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1246:1: (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE )
            int alt17=4;
            switch ( input.LA(1) ) {
            case RULE_IFDEF:
                {
                alt17=1;
                }
                break;
            case RULE_IFNDEF:
                {
                alt17=2;
                }
                break;
            case RULE_IFHAVE:
                {
                alt17=3;
                }
                break;
            case RULE_IFNHAVE:
                {
                alt17=4;
                }
                break;
            default:
                NoViableAltException nvae =
                    new NoViableAltException("1246:1: (lv_name_0_1= RULE_IFDEF | lv_name_0_2= RULE_IFNDEF | lv_name_0_3= RULE_IFHAVE | lv_name_0_4= RULE_IFNHAVE )", 17, 0, input);

                throw nvae;
            }

            switch (alt17) {
                case 1 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1247:3: lv_name_0_1= RULE_IFDEF
                    {
                    lv_name_0_1=(Token)input.LT(1);
                    match(input,RULE_IFDEF,FOLLOW_RULE_IFDEF_in_ruleWMLPreprocIF1849); 

                    			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getNameIFDEFTerminalRuleCall_0_0_0(), "name"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"name",
                    	        		lv_name_0_1, 
                    	        		"IFDEF", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 2 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1268:8: lv_name_0_2= RULE_IFNDEF
                    {
                    lv_name_0_2=(Token)input.LT(1);
                    match(input,RULE_IFNDEF,FOLLOW_RULE_IFNDEF_in_ruleWMLPreprocIF1869); 

                    			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getNameIFNDEFTerminalRuleCall_0_0_1(), "name"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"name",
                    	        		lv_name_0_2, 
                    	        		"IFNDEF", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 3 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1289:8: lv_name_0_3= RULE_IFHAVE
                    {
                    lv_name_0_3=(Token)input.LT(1);
                    match(input,RULE_IFHAVE,FOLLOW_RULE_IFHAVE_in_ruleWMLPreprocIF1889); 

                    			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getNameIFHAVETerminalRuleCall_0_0_2(), "name"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"name",
                    	        		lv_name_0_3, 
                    	        		"IFHAVE", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 4 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1310:8: lv_name_0_4= RULE_IFNHAVE
                    {
                    lv_name_0_4=(Token)input.LT(1);
                    match(input,RULE_IFNHAVE,FOLLOW_RULE_IFNHAVE_in_ruleWMLPreprocIF1909); 

                    			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getNameIFNHAVETerminalRuleCall_0_0_3(), "name"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"name",
                    	        		lv_name_0_4, 
                    	        		"IFNHAVE", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;

            }


            }


            }

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1334:2: ( ( (lv_Tags_1_0= ruleWMLTag ) ) | ( (lv_Keys_2_0= ruleWMLKey ) ) | ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) ) | ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) ) | ( (lv_Textdomains_5_0= ruleWMLTextdomain ) ) | ( (lv_Values_6_0= ruleWMLValue ) ) | ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) ) | ( (lv_Elses_8_0= RULE_ELSE ) ) )*
            loop18:
            do {
                int alt18=9;
                switch ( input.LA(1) ) {
                case 20:
                    {
                    alt18=1;
                    }
                    break;
                case RULE_ID:
                    {
                    int LA18_3 = input.LA(2);

                    if ( (LA18_3==24) ) {
                        alt18=2;
                    }
                    else if ( (LA18_3==RULE_ID||LA18_3==RULE_DEFINE||(LA18_3>=RULE_IFDEF && LA18_3<=RULE_ANY_OTHER)||LA18_3==20||(LA18_3>=25 && LA18_3<=27)||(LA18_3>=29 && LA18_3<=34)) ) {
                        alt18=6;
                    }


                    }
                    break;
                case 25:
                    {
                    alt18=3;
                    }
                    break;
                case RULE_DEFINE:
                    {
                    alt18=4;
                    }
                    break;
                case RULE_TEXTDOMAIN:
                    {
                    alt18=5;
                    }
                    break;
                case RULE_STRING:
                case RULE_ANY_OTHER:
                case 26:
                case 27:
                case 29:
                case 30:
                case 31:
                case 32:
                case 33:
                case 34:
                    {
                    alt18=6;
                    }
                    break;
                case RULE_IFDEF:
                case RULE_IFNDEF:
                case RULE_IFHAVE:
                case RULE_IFNHAVE:
                    {
                    alt18=7;
                    }
                    break;
                case RULE_ELSE:
                    {
                    alt18=8;
                    }
                    break;

                }

                switch (alt18) {
            	case 1 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1334:3: ( (lv_Tags_1_0= ruleWMLTag ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1334:3: ( (lv_Tags_1_0= ruleWMLTag ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1335:1: (lv_Tags_1_0= ruleWMLTag )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1335:1: (lv_Tags_1_0= ruleWMLTag )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1336:3: lv_Tags_1_0= ruleWMLTag
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLPreprocIFAccess().getTagsWMLTagParserRuleCall_1_0_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLTag_in_ruleWMLPreprocIF1939);
            	    lv_Tags_1_0=ruleWMLTag();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Tags",
            	    	        		lv_Tags_1_0, 
            	    	        		"WMLTag", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 2 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1359:6: ( (lv_Keys_2_0= ruleWMLKey ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1359:6: ( (lv_Keys_2_0= ruleWMLKey ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1360:1: (lv_Keys_2_0= ruleWMLKey )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1360:1: (lv_Keys_2_0= ruleWMLKey )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1361:3: lv_Keys_2_0= ruleWMLKey
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLPreprocIFAccess().getKeysWMLKeyParserRuleCall_1_1_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLKey_in_ruleWMLPreprocIF1966);
            	    lv_Keys_2_0=ruleWMLKey();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Keys",
            	    	        		lv_Keys_2_0, 
            	    	        		"WMLKey", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 3 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1384:6: ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1384:6: ( (lv_MacroCalls_3_0= ruleWMLMacroCall ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1385:1: (lv_MacroCalls_3_0= ruleWMLMacroCall )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1385:1: (lv_MacroCalls_3_0= ruleWMLMacroCall )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1386:3: lv_MacroCalls_3_0= ruleWMLMacroCall
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLPreprocIFAccess().getMacroCallsWMLMacroCallParserRuleCall_1_2_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroCall_in_ruleWMLPreprocIF1993);
            	    lv_MacroCalls_3_0=ruleWMLMacroCall();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"MacroCalls",
            	    	        		lv_MacroCalls_3_0, 
            	    	        		"WMLMacroCall", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 4 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1409:6: ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1409:6: ( (lv_MacroDefines_4_0= ruleWMLMacroDefine ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1410:1: (lv_MacroDefines_4_0= ruleWMLMacroDefine )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1410:1: (lv_MacroDefines_4_0= ruleWMLMacroDefine )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1411:3: lv_MacroDefines_4_0= ruleWMLMacroDefine
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLPreprocIFAccess().getMacroDefinesWMLMacroDefineParserRuleCall_1_3_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLMacroDefine_in_ruleWMLPreprocIF2020);
            	    lv_MacroDefines_4_0=ruleWMLMacroDefine();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"MacroDefines",
            	    	        		lv_MacroDefines_4_0, 
            	    	        		"WMLMacroDefine", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 5 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1434:6: ( (lv_Textdomains_5_0= ruleWMLTextdomain ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1434:6: ( (lv_Textdomains_5_0= ruleWMLTextdomain ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1435:1: (lv_Textdomains_5_0= ruleWMLTextdomain )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1435:1: (lv_Textdomains_5_0= ruleWMLTextdomain )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1436:3: lv_Textdomains_5_0= ruleWMLTextdomain
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLPreprocIFAccess().getTextdomainsWMLTextdomainParserRuleCall_1_4_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLTextdomain_in_ruleWMLPreprocIF2047);
            	    lv_Textdomains_5_0=ruleWMLTextdomain();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Textdomains",
            	    	        		lv_Textdomains_5_0, 
            	    	        		"WMLTextdomain", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 6 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1459:6: ( (lv_Values_6_0= ruleWMLValue ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1459:6: ( (lv_Values_6_0= ruleWMLValue ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1460:1: (lv_Values_6_0= ruleWMLValue )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1460:1: (lv_Values_6_0= ruleWMLValue )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1461:3: lv_Values_6_0= ruleWMLValue
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLPreprocIFAccess().getValuesWMLValueParserRuleCall_1_5_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLValue_in_ruleWMLPreprocIF2074);
            	    lv_Values_6_0=ruleWMLValue();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Values",
            	    	        		lv_Values_6_0, 
            	    	        		"WMLValue", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 7 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1484:6: ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1484:6: ( (lv_IfDefs_7_0= ruleWMLPreprocIF ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1485:1: (lv_IfDefs_7_0= ruleWMLPreprocIF )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1485:1: (lv_IfDefs_7_0= ruleWMLPreprocIF )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1486:3: lv_IfDefs_7_0= ruleWMLPreprocIF
            	    {
            	     
            	    	        currentNode=createCompositeNode(grammarAccess.getWMLPreprocIFAccess().getIfDefsWMLPreprocIFParserRuleCall_1_6_0(), currentNode); 
            	    	    
            	    pushFollow(FOLLOW_ruleWMLPreprocIF_in_ruleWMLPreprocIF2101);
            	    lv_IfDefs_7_0=ruleWMLPreprocIF();
            	    _fsp--;


            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode.getParent(), current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"IfDefs",
            	    	        		lv_IfDefs_7_0, 
            	    	        		"WMLPreprocIF", 
            	    	        		currentNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	        currentNode = currentNode.getParent();
            	    	    

            	    }


            	    }


            	    }
            	    break;
            	case 8 :
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1509:6: ( (lv_Elses_8_0= RULE_ELSE ) )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1509:6: ( (lv_Elses_8_0= RULE_ELSE ) )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1510:1: (lv_Elses_8_0= RULE_ELSE )
            	    {
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1510:1: (lv_Elses_8_0= RULE_ELSE )
            	    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1511:3: lv_Elses_8_0= RULE_ELSE
            	    {
            	    lv_Elses_8_0=(Token)input.LT(1);
            	    match(input,RULE_ELSE,FOLLOW_RULE_ELSE_in_ruleWMLPreprocIF2124); 

            	    			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getElsesELSETerminalRuleCall_1_7_0(), "Elses"); 
            	    		

            	    	        if (current==null) {
            	    	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	    	            associateNodeWithAstElement(currentNode, current);
            	    	        }
            	    	        try {
            	    	       		add(
            	    	       			current, 
            	    	       			"Elses",
            	    	        		lv_Elses_8_0, 
            	    	        		"ELSE", 
            	    	        		lastConsumedNode);
            	    	        } catch (ValueConverterException vce) {
            	    				handleValueConverterException(vce);
            	    	        }
            	    	    

            	    }


            	    }


            	    }
            	    break;

            	default :
            	    break loop18;
                }
            } while (true);

            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1533:4: ( (lv_endName_9_0= RULE_ENDIF ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1534:1: (lv_endName_9_0= RULE_ENDIF )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1534:1: (lv_endName_9_0= RULE_ENDIF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1535:3: lv_endName_9_0= RULE_ENDIF
            {
            lv_endName_9_0=(Token)input.LT(1);
            match(input,RULE_ENDIF,FOLLOW_RULE_ENDIF_in_ruleWMLPreprocIF2148); 

            			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getEndNameENDIFTerminalRuleCall_2_0(), "endName"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"endName",
            	        		lv_endName_9_0, 
            	        		"ENDIF", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }


            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLPreprocIF


    // $ANTLR start entryRuleWMLTextdomain
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1565:1: entryRuleWMLTextdomain returns [EObject current=null] : iv_ruleWMLTextdomain= ruleWMLTextdomain EOF ;
    public final EObject entryRuleWMLTextdomain() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLTextdomain = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1566:2: (iv_ruleWMLTextdomain= ruleWMLTextdomain EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1567:2: iv_ruleWMLTextdomain= ruleWMLTextdomain EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLTextdomainRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLTextdomain_in_entryRuleWMLTextdomain2189);
            iv_ruleWMLTextdomain=ruleWMLTextdomain();
            _fsp--;

             current =iv_ruleWMLTextdomain; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLTextdomain2199); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLTextdomain


    // $ANTLR start ruleWMLTextdomain
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1574:1: ruleWMLTextdomain returns [EObject current=null] : ( (lv_name_0_0= RULE_TEXTDOMAIN ) ) ;
    public final EObject ruleWMLTextdomain() throws RecognitionException {
        EObject current = null;

        Token lv_name_0_0=null;

         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1579:6: ( ( (lv_name_0_0= RULE_TEXTDOMAIN ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1580:1: ( (lv_name_0_0= RULE_TEXTDOMAIN ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1580:1: ( (lv_name_0_0= RULE_TEXTDOMAIN ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1581:1: (lv_name_0_0= RULE_TEXTDOMAIN )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1581:1: (lv_name_0_0= RULE_TEXTDOMAIN )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1582:3: lv_name_0_0= RULE_TEXTDOMAIN
            {
            lv_name_0_0=(Token)input.LT(1);
            match(input,RULE_TEXTDOMAIN,FOLLOW_RULE_TEXTDOMAIN_in_ruleWMLTextdomain2240); 

            			createLeafNode(grammarAccess.getWMLTextdomainAccess().getNameTEXTDOMAINTerminalRuleCall_0(), "name"); 
            		

            	        if (current==null) {
            	            current = factory.create(grammarAccess.getWMLTextdomainRule().getType().getClassifier());
            	            associateNodeWithAstElement(currentNode, current);
            	        }
            	        try {
            	       		set(
            	       			current, 
            	       			"name",
            	        		lv_name_0_0, 
            	        		"TEXTDOMAIN", 
            	        		lastConsumedNode);
            	        } catch (ValueConverterException vce) {
            				handleValueConverterException(vce);
            	        }
            	    

            }


            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLTextdomain


    // $ANTLR start entryRuleWMLValue
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1612:1: entryRuleWMLValue returns [EObject current=null] : iv_ruleWMLValue= ruleWMLValue EOF ;
    public final EObject entryRuleWMLValue() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleWMLValue = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1613:2: (iv_ruleWMLValue= ruleWMLValue EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1614:2: iv_ruleWMLValue= ruleWMLValue EOF
            {
             currentNode = createCompositeNode(grammarAccess.getWMLValueRule(), currentNode); 
            pushFollow(FOLLOW_ruleWMLValue_in_entryRuleWMLValue2280);
            iv_ruleWMLValue=ruleWMLValue();
            _fsp--;

             current =iv_ruleWMLValue; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleWMLValue2290); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleWMLValue


    // $ANTLR start ruleWMLValue
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1621:1: ruleWMLValue returns [EObject current=null] : ( ( (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER ) ) ) ;
    public final EObject ruleWMLValue() throws RecognitionException {
        EObject current = null;

        Token lv_value_0_1=null;
        Token lv_value_0_2=null;
        Token lv_value_0_3=null;
        Token lv_value_0_4=null;
        Token lv_value_0_5=null;
        Token lv_value_0_6=null;
        Token lv_value_0_7=null;
        Token lv_value_0_8=null;
        Token lv_value_0_9=null;
        Token lv_value_0_10=null;
        Token lv_value_0_11=null;

         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1626:6: ( ( ( (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER ) ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1627:1: ( ( (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER ) ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1627:1: ( ( (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1628:1: ( (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1628:1: ( (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1629:1: (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1629:1: (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER )
            int alt19=11;
            switch ( input.LA(1) ) {
            case RULE_ID:
                {
                alt19=1;
                }
                break;
            case RULE_STRING:
                {
                alt19=2;
                }
                break;
            case 29:
                {
                alt19=3;
                }
                break;
            case 27:
                {
                alt19=4;
                }
                break;
            case 30:
                {
                alt19=5;
                }
                break;
            case 26:
                {
                alt19=6;
                }
                break;
            case 31:
                {
                alt19=7;
                }
                break;
            case 32:
                {
                alt19=8;
                }
                break;
            case 33:
                {
                alt19=9;
                }
                break;
            case 34:
                {
                alt19=10;
                }
                break;
            case RULE_ANY_OTHER:
                {
                alt19=11;
                }
                break;
            default:
                NoViableAltException nvae =
                    new NoViableAltException("1629:1: (lv_value_0_1= RULE_ID | lv_value_0_2= RULE_STRING | lv_value_0_3= '_' | lv_value_0_4= '~' | lv_value_0_5= '.' | lv_value_0_6= './' | lv_value_0_7= '$' | lv_value_0_8= '/' | lv_value_0_9= '(' | lv_value_0_10= ')' | lv_value_0_11= RULE_ANY_OTHER )", 19, 0, input);

                throw nvae;
            }

            switch (alt19) {
                case 1 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1630:3: lv_value_0_1= RULE_ID
                    {
                    lv_value_0_1=(Token)input.LT(1);
                    match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleWMLValue2333); 

                    			createLeafNode(grammarAccess.getWMLValueAccess().getValueIDTerminalRuleCall_0_0(), "value"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"value",
                    	        		lv_value_0_1, 
                    	        		"ID", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 2 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1651:8: lv_value_0_2= RULE_STRING
                    {
                    lv_value_0_2=(Token)input.LT(1);
                    match(input,RULE_STRING,FOLLOW_RULE_STRING_in_ruleWMLValue2353); 

                    			createLeafNode(grammarAccess.getWMLValueAccess().getValueSTRINGTerminalRuleCall_0_1(), "value"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"value",
                    	        		lv_value_0_2, 
                    	        		"STRING", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 3 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1672:8: lv_value_0_3= '_'
                    {
                    lv_value_0_3=(Token)input.LT(1);
                    match(input,29,FOLLOW_29_in_ruleWMLValue2374); 

                            createLeafNode(grammarAccess.getWMLValueAccess().getValue_Keyword_0_2(), "value"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "value", lv_value_0_3, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 4 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1690:8: lv_value_0_4= '~'
                    {
                    lv_value_0_4=(Token)input.LT(1);
                    match(input,27,FOLLOW_27_in_ruleWMLValue2403); 

                            createLeafNode(grammarAccess.getWMLValueAccess().getValueTildeKeyword_0_3(), "value"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "value", lv_value_0_4, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 5 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1708:8: lv_value_0_5= '.'
                    {
                    lv_value_0_5=(Token)input.LT(1);
                    match(input,30,FOLLOW_30_in_ruleWMLValue2432); 

                            createLeafNode(grammarAccess.getWMLValueAccess().getValueFullStopKeyword_0_4(), "value"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "value", lv_value_0_5, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 6 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1726:8: lv_value_0_6= './'
                    {
                    lv_value_0_6=(Token)input.LT(1);
                    match(input,26,FOLLOW_26_in_ruleWMLValue2461); 

                            createLeafNode(grammarAccess.getWMLValueAccess().getValueFullStopSolidusKeyword_0_5(), "value"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "value", lv_value_0_6, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 7 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1744:8: lv_value_0_7= '$'
                    {
                    lv_value_0_7=(Token)input.LT(1);
                    match(input,31,FOLLOW_31_in_ruleWMLValue2490); 

                            createLeafNode(grammarAccess.getWMLValueAccess().getValueDollarSignKeyword_0_6(), "value"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "value", lv_value_0_7, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 8 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1762:8: lv_value_0_8= '/'
                    {
                    lv_value_0_8=(Token)input.LT(1);
                    match(input,32,FOLLOW_32_in_ruleWMLValue2519); 

                            createLeafNode(grammarAccess.getWMLValueAccess().getValueSolidusKeyword_0_7(), "value"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "value", lv_value_0_8, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 9 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1780:8: lv_value_0_9= '('
                    {
                    lv_value_0_9=(Token)input.LT(1);
                    match(input,33,FOLLOW_33_in_ruleWMLValue2548); 

                            createLeafNode(grammarAccess.getWMLValueAccess().getValueLeftParenthesisKeyword_0_8(), "value"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "value", lv_value_0_9, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 10 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1798:8: lv_value_0_10= ')'
                    {
                    lv_value_0_10=(Token)input.LT(1);
                    match(input,34,FOLLOW_34_in_ruleWMLValue2577); 

                            createLeafNode(grammarAccess.getWMLValueAccess().getValueRightParenthesisKeyword_0_9(), "value"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "value", lv_value_0_10, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 11 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1816:8: lv_value_0_11= RULE_ANY_OTHER
                    {
                    lv_value_0_11=(Token)input.LT(1);
                    match(input,RULE_ANY_OTHER,FOLLOW_RULE_ANY_OTHER_in_ruleWMLValue2605); 

                    			createLeafNode(grammarAccess.getWMLValueAccess().getValueANY_OTHERTerminalRuleCall_0_10(), "value"); 
                    		

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getWMLValueRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        try {
                    	       		set(
                    	       			current, 
                    	       			"value",
                    	        		lv_value_0_11, 
                    	        		"ANY_OTHER", 
                    	        		lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;

            }


            }


            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleWMLValue


    // $ANTLR start entryRuleMacroTokens
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1848:1: entryRuleMacroTokens returns [EObject current=null] : iv_ruleMacroTokens= ruleMacroTokens EOF ;
    public final EObject entryRuleMacroTokens() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleMacroTokens = null;


        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1849:2: (iv_ruleMacroTokens= ruleMacroTokens EOF )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1850:2: iv_ruleMacroTokens= ruleMacroTokens EOF
            {
             currentNode = createCompositeNode(grammarAccess.getMacroTokensRule(), currentNode); 
            pushFollow(FOLLOW_ruleMacroTokens_in_entryRuleMacroTokens2648);
            iv_ruleMacroTokens=ruleMacroTokens();
            _fsp--;

             current =iv_ruleMacroTokens; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleMacroTokens2658); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end entryRuleMacroTokens


    // $ANTLR start ruleMacroTokens
    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1857:1: ruleMacroTokens returns [EObject current=null] : ( ( (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' ) ) ) ;
    public final EObject ruleMacroTokens() throws RecognitionException {
        EObject current = null;

        Token lv_val_0_1=null;
        Token lv_val_0_2=null;
        Token lv_val_0_3=null;
        Token lv_val_0_4=null;
        Token lv_val_0_5=null;

         EObject temp=null; setCurrentLookahead(); resetLookahead(); 
            
        try {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1862:6: ( ( ( (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' ) ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1863:1: ( ( (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' ) ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1863:1: ( ( (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' ) ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1864:1: ( (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' ) )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1864:1: ( (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' ) )
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1865:1: (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' )
            {
            // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1865:1: (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' )
            int alt20=5;
            switch ( input.LA(1) ) {
            case 24:
                {
                alt20=1;
                }
                break;
            case 20:
                {
                alt20=2;
                }
                break;
            case 22:
                {
                alt20=3;
                }
                break;
            case 21:
                {
                alt20=4;
                }
                break;
            case 23:
                {
                alt20=5;
                }
                break;
            default:
                NoViableAltException nvae =
                    new NoViableAltException("1865:1: (lv_val_0_1= '=' | lv_val_0_2= '[' | lv_val_0_3= ']' | lv_val_0_4= '+' | lv_val_0_5= '[/' )", 20, 0, input);

                throw nvae;
            }

            switch (alt20) {
                case 1 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1866:3: lv_val_0_1= '='
                    {
                    lv_val_0_1=(Token)input.LT(1);
                    match(input,24,FOLLOW_24_in_ruleMacroTokens2702); 

                            createLeafNode(grammarAccess.getMacroTokensAccess().getValEqualsSignKeyword_0_0(), "val"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getMacroTokensRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "val", lv_val_0_1, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 2 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1884:8: lv_val_0_2= '['
                    {
                    lv_val_0_2=(Token)input.LT(1);
                    match(input,20,FOLLOW_20_in_ruleMacroTokens2731); 

                            createLeafNode(grammarAccess.getMacroTokensAccess().getValLeftSquareBracketKeyword_0_1(), "val"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getMacroTokensRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "val", lv_val_0_2, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 3 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1902:8: lv_val_0_3= ']'
                    {
                    lv_val_0_3=(Token)input.LT(1);
                    match(input,22,FOLLOW_22_in_ruleMacroTokens2760); 

                            createLeafNode(grammarAccess.getMacroTokensAccess().getValRightSquareBracketKeyword_0_2(), "val"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getMacroTokensRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "val", lv_val_0_3, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 4 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1920:8: lv_val_0_4= '+'
                    {
                    lv_val_0_4=(Token)input.LT(1);
                    match(input,21,FOLLOW_21_in_ruleMacroTokens2789); 

                            createLeafNode(grammarAccess.getMacroTokensAccess().getValPlusSignKeyword_0_3(), "val"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getMacroTokensRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "val", lv_val_0_4, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;
                case 5 :
                    // ../org.wesnoth.wml/src-gen/org/wesnoth/parser/antlr/internal/InternalWML.g:1938:8: lv_val_0_5= '[/'
                    {
                    lv_val_0_5=(Token)input.LT(1);
                    match(input,23,FOLLOW_23_in_ruleMacroTokens2818); 

                            createLeafNode(grammarAccess.getMacroTokensAccess().getValLeftSquareBracketSolidusKeyword_0_4(), "val"); 
                        

                    	        if (current==null) {
                    	            current = factory.create(grammarAccess.getMacroTokensRule().getType().getClassifier());
                    	            associateNodeWithAstElement(currentNode, current);
                    	        }
                    	        
                    	        try {
                    	       		set(current, "val", lv_val_0_5, null, lastConsumedNode);
                    	        } catch (ValueConverterException vce) {
                    				handleValueConverterException(vce);
                    	        }
                    	    

                    }
                    break;

            }


            }


            }


            }

             resetLookahead(); 
                	lastConsumedNode = currentNode;
                
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end ruleMacroTokens


 

    public static final BitSet FOLLOW_ruleWMLRoot_in_entryRuleWMLRoot75 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLRoot85 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLTag_in_ruleWMLRoot131 = new BitSet(new long[]{0x0000000002113D02L});
    public static final BitSet FOLLOW_ruleWMLMacroCall_in_ruleWMLRoot158 = new BitSet(new long[]{0x0000000002113D02L});
    public static final BitSet FOLLOW_ruleWMLMacroDefine_in_ruleWMLRoot185 = new BitSet(new long[]{0x0000000002113D02L});
    public static final BitSet FOLLOW_ruleWMLTextdomain_in_ruleWMLRoot212 = new BitSet(new long[]{0x0000000002113D02L});
    public static final BitSet FOLLOW_ruleWMLPreprocIF_in_ruleWMLRoot239 = new BitSet(new long[]{0x0000000002113D02L});
    public static final BitSet FOLLOW_ruleWMLTag_in_entryRuleWMLTag276 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLTag286 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_20_in_ruleWMLTag321 = new BitSet(new long[]{0x0000000000200010L});
    public static final BitSet FOLLOW_21_in_ruleWMLTag339 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleWMLTag370 = new BitSet(new long[]{0x0000000000400000L});
    public static final BitSet FOLLOW_22_in_ruleWMLTag385 = new BitSet(new long[]{0x0000000002913D10L});
    public static final BitSet FOLLOW_ruleWMLTag_in_ruleWMLTag407 = new BitSet(new long[]{0x0000000002913D10L});
    public static final BitSet FOLLOW_ruleWMLKey_in_ruleWMLTag434 = new BitSet(new long[]{0x0000000002913D10L});
    public static final BitSet FOLLOW_ruleWMLMacroCall_in_ruleWMLTag461 = new BitSet(new long[]{0x0000000002913D10L});
    public static final BitSet FOLLOW_ruleWMLMacroDefine_in_ruleWMLTag488 = new BitSet(new long[]{0x0000000002913D10L});
    public static final BitSet FOLLOW_ruleWMLTextdomain_in_ruleWMLTag515 = new BitSet(new long[]{0x0000000002913D10L});
    public static final BitSet FOLLOW_ruleWMLPreprocIF_in_ruleWMLTag542 = new BitSet(new long[]{0x0000000002913D10L});
    public static final BitSet FOLLOW_23_in_ruleWMLTag554 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleWMLTag571 = new BitSet(new long[]{0x0000000000400000L});
    public static final BitSet FOLLOW_22_in_ruleWMLTag586 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLKey_in_entryRuleWMLKey628 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLKey638 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleWMLKey684 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_24_in_ruleWMLKey699 = new BitSet(new long[]{0x00000007EE3600F0L});
    public static final BitSet FOLLOW_ruleWMLKeyValue_in_ruleWMLKey720 = new BitSet(new long[]{0x00000007EE3600F0L});
    public static final BitSet FOLLOW_RULE_EOL_in_ruleWMLKey732 = new BitSet(new long[]{0x0000000000200000L});
    public static final BitSet FOLLOW_21_in_ruleWMLKey743 = new BitSet(new long[]{0x00000007EE1600B0L});
    public static final BitSet FOLLOW_RULE_EOL_in_ruleWMLKey753 = new BitSet(new long[]{0x00000007EE160090L});
    public static final BitSet FOLLOW_ruleWMLKeyValue_in_ruleWMLKey775 = new BitSet(new long[]{0x00000007EE3600F0L});
    public static final BitSet FOLLOW_RULE_EOL_in_ruleWMLKey797 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_SL_COMMENT_in_ruleWMLKey817 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLKeyValue_in_entryRuleWMLKeyValue865 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLKeyValue875 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLValue_in_ruleWMLKeyValue922 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLMacroCall_in_ruleWMLKeyValue949 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLLuaCode_in_ruleWMLKeyValue976 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLArrayCall_in_ruleWMLKeyValue1003 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLMacroCall_in_entryRuleWMLMacroCall1038 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLMacroCall1048 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_25_in_ruleWMLMacroCall1083 = new BitSet(new long[]{0x000000000C000010L});
    public static final BitSet FOLLOW_26_in_ruleWMLMacroCall1101 = new BitSet(new long[]{0x0000000008000010L});
    public static final BitSet FOLLOW_27_in_ruleWMLMacroCall1133 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleWMLMacroCall1164 = new BitSet(new long[]{0x00000007FFF60010L});
    public static final BitSet FOLLOW_ruleWMLValue_in_ruleWMLMacroCall1193 = new BitSet(new long[]{0x00000007FFF60010L});
    public static final BitSet FOLLOW_ruleMacroTokens_in_ruleWMLMacroCall1212 = new BitSet(new long[]{0x00000007FFF60010L});
    public static final BitSet FOLLOW_ruleWMLMacroCall_in_ruleWMLMacroCall1242 = new BitSet(new long[]{0x00000007FFF60010L});
    public static final BitSet FOLLOW_28_in_ruleWMLMacroCall1254 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLLuaCode_in_entryRuleWMLLuaCode1290 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLLuaCode1300 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_LUA_CODE_in_ruleWMLLuaCode1341 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLArrayCall_in_entryRuleWMLArrayCall1381 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLArrayCall1391 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_20_in_ruleWMLArrayCall1426 = new BitSet(new long[]{0x00000007EC060010L});
    public static final BitSet FOLLOW_ruleWMLValue_in_ruleWMLArrayCall1447 = new BitSet(new long[]{0x00000007EC460010L});
    public static final BitSet FOLLOW_22_in_ruleWMLArrayCall1458 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLMacroDefine_in_entryRuleWMLMacroDefine1494 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLMacroDefine1504 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_DEFINE_in_ruleWMLMacroDefine1546 = new BitSet(new long[]{0x00000007EE173F10L});
    public static final BitSet FOLLOW_ruleWMLTag_in_ruleWMLMacroDefine1573 = new BitSet(new long[]{0x00000007EE173F10L});
    public static final BitSet FOLLOW_ruleWMLKey_in_ruleWMLMacroDefine1600 = new BitSet(new long[]{0x00000007EE173F10L});
    public static final BitSet FOLLOW_ruleWMLMacroCall_in_ruleWMLMacroDefine1627 = new BitSet(new long[]{0x00000007EE173F10L});
    public static final BitSet FOLLOW_ruleWMLMacroDefine_in_ruleWMLMacroDefine1654 = new BitSet(new long[]{0x00000007EE173F10L});
    public static final BitSet FOLLOW_ruleWMLTextdomain_in_ruleWMLMacroDefine1681 = new BitSet(new long[]{0x00000007EE173F10L});
    public static final BitSet FOLLOW_ruleWMLValue_in_ruleWMLMacroDefine1708 = new BitSet(new long[]{0x00000007EE173F10L});
    public static final BitSet FOLLOW_ruleWMLPreprocIF_in_ruleWMLMacroDefine1735 = new BitSet(new long[]{0x00000007EE173F10L});
    public static final BitSet FOLLOW_RULE_ENDDEF_in_ruleWMLMacroDefine1754 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLPreprocIF_in_entryRuleWMLPreprocIF1795 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLPreprocIF1805 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_IFDEF_in_ruleWMLPreprocIF1849 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_RULE_IFNDEF_in_ruleWMLPreprocIF1869 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_RULE_IFHAVE_in_ruleWMLPreprocIF1889 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_RULE_IFNHAVE_in_ruleWMLPreprocIF1909 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_ruleWMLTag_in_ruleWMLPreprocIF1939 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_ruleWMLKey_in_ruleWMLPreprocIF1966 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_ruleWMLMacroCall_in_ruleWMLPreprocIF1993 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_ruleWMLMacroDefine_in_ruleWMLPreprocIF2020 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_ruleWMLTextdomain_in_ruleWMLPreprocIF2047 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_ruleWMLValue_in_ruleWMLPreprocIF2074 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_ruleWMLPreprocIF_in_ruleWMLPreprocIF2101 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_RULE_ELSE_in_ruleWMLPreprocIF2124 = new BitSet(new long[]{0x00000007EE17FD10L});
    public static final BitSet FOLLOW_RULE_ENDIF_in_ruleWMLPreprocIF2148 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLTextdomain_in_entryRuleWMLTextdomain2189 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLTextdomain2199 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_TEXTDOMAIN_in_ruleWMLTextdomain2240 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleWMLValue_in_entryRuleWMLValue2280 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleWMLValue2290 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleWMLValue2333 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_STRING_in_ruleWMLValue2353 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_29_in_ruleWMLValue2374 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_27_in_ruleWMLValue2403 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_30_in_ruleWMLValue2432 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_26_in_ruleWMLValue2461 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_31_in_ruleWMLValue2490 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_32_in_ruleWMLValue2519 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_33_in_ruleWMLValue2548 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_34_in_ruleWMLValue2577 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_ANY_OTHER_in_ruleWMLValue2605 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleMacroTokens_in_entryRuleMacroTokens2648 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleMacroTokens2658 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_24_in_ruleMacroTokens2702 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_20_in_ruleMacroTokens2731 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_22_in_ruleMacroTokens2760 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_21_in_ruleMacroTokens2789 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_23_in_ruleMacroTokens2818 = new BitSet(new long[]{0x0000000000000002L});

}