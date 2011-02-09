/**
 * <copyright>
 * </copyright>
 *

 */
package org.wesnoth.wml.impl;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

import org.eclipse.emf.ecore.impl.EPackageImpl;

import org.wesnoth.wml.MacroTokens;
import org.wesnoth.wml.WMLArrayCall;
import org.wesnoth.wml.WMLKey;
import org.wesnoth.wml.WMLKeyValue;
import org.wesnoth.wml.WMLLuaCode;
import org.wesnoth.wml.WMLMacroCall;
import org.wesnoth.wml.WMLMacroDefine;
import org.wesnoth.wml.WMLPreprocIF;
import org.wesnoth.wml.WMLRoot;
import org.wesnoth.wml.WMLTag;
import org.wesnoth.wml.WMLTextdomain;
import org.wesnoth.wml.WMLValue;
import org.wesnoth.wml.WmlFactory;
import org.wesnoth.wml.WmlPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Package</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class WmlPackageImpl extends EPackageImpl implements WmlPackage
{
  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlRootEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlTagEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlKeyEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlKeyValueEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlMacroCallEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlLuaCodeEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlArrayCallEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlMacroDefineEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlPreprocIFEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlTextdomainEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass wmlValueEClass = null;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private EClass macroTokensEClass = null;

  /**
   * Creates an instance of the model <b>Package</b>, registered with
   * {@link org.eclipse.emf.ecore.EPackage.Registry EPackage.Registry} by the package
   * package URI value.
   * <p>Note: the correct way to create the package is via the static
   * factory method {@link #init init()}, which also performs
   * initialization of the package, or returns the registered package,
   * if one already exists.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.eclipse.emf.ecore.EPackage.Registry
   * @see org.wesnoth.wml.WmlPackage#eNS_URI
   * @see #init()
   * @generated
   */
  private WmlPackageImpl()
  {
    super(eNS_URI, WmlFactory.eINSTANCE);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private static boolean isInited = false;

  /**
   * Creates, registers, and initializes the <b>Package</b> for this model, and for any others upon which it depends.
   * 
   * <p>This method is used to initialize {@link WmlPackage#eINSTANCE} when that field is accessed.
   * Clients should not invoke it directly. Instead, they should simply access that field to obtain the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #eNS_URI
   * @see #createPackageContents()
   * @see #initializePackageContents()
   * @generated
   */
  public static WmlPackage init()
  {
    if (isInited) return (WmlPackage)EPackage.Registry.INSTANCE.getEPackage(WmlPackage.eNS_URI);

    // Obtain or create and register package
    WmlPackageImpl theWmlPackage = (WmlPackageImpl)(EPackage.Registry.INSTANCE.get(eNS_URI) instanceof WmlPackageImpl ? EPackage.Registry.INSTANCE.get(eNS_URI) : new WmlPackageImpl());

    isInited = true;

    // Create package meta-data objects
    theWmlPackage.createPackageContents();

    // Initialize created meta-data
    theWmlPackage.initializePackageContents();

    // Mark meta-data to indicate it can't be changed
    theWmlPackage.freeze();

  
    // Update the registry and return the package
    EPackage.Registry.INSTANCE.put(WmlPackage.eNS_URI, theWmlPackage);
    return theWmlPackage;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLRoot()
  {
    return wmlRootEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLRoot_Tags()
  {
    return (EReference)wmlRootEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLRoot_MacroCalls()
  {
    return (EReference)wmlRootEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLRoot_MacroDefines()
  {
    return (EReference)wmlRootEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLRoot_Textdomains()
  {
    return (EReference)wmlRootEClass.getEStructuralFeatures().get(3);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLRoot_IfDefs()
  {
    return (EReference)wmlRootEClass.getEStructuralFeatures().get(4);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLTag()
  {
    return wmlTagEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLTag_Plus()
  {
    return (EAttribute)wmlTagEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLTag_Name()
  {
    return (EAttribute)wmlTagEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLTag_Tags()
  {
    return (EReference)wmlTagEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLTag_Keys()
  {
    return (EReference)wmlTagEClass.getEStructuralFeatures().get(3);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLTag_MacroCalls()
  {
    return (EReference)wmlTagEClass.getEStructuralFeatures().get(4);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLTag_MacroDefines()
  {
    return (EReference)wmlTagEClass.getEStructuralFeatures().get(5);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLTag_Textdomains()
  {
    return (EReference)wmlTagEClass.getEStructuralFeatures().get(6);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLTag_IfDefs()
  {
    return (EReference)wmlTagEClass.getEStructuralFeatures().get(7);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLTag_EndName()
  {
    return (EAttribute)wmlTagEClass.getEStructuralFeatures().get(8);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLKey()
  {
    return wmlKeyEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLKey_Name()
  {
    return (EAttribute)wmlKeyEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLKey_Value()
  {
    return (EReference)wmlKeyEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLKey_Eol()
  {
    return (EAttribute)wmlKeyEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLKeyValue()
  {
    return wmlKeyValueEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLMacroCall()
  {
    return wmlMacroCallEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLMacroCall_Point()
  {
    return (EAttribute)wmlMacroCallEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLMacroCall_Relative()
  {
    return (EAttribute)wmlMacroCallEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLMacroCall_Name()
  {
    return (EAttribute)wmlMacroCallEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroCall_Params()
  {
    return (EReference)wmlMacroCallEClass.getEStructuralFeatures().get(3);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroCall_ExtraMacros()
  {
    return (EReference)wmlMacroCallEClass.getEStructuralFeatures().get(4);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLLuaCode()
  {
    return wmlLuaCodeEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLLuaCode_Value()
  {
    return (EAttribute)wmlLuaCodeEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLArrayCall()
  {
    return wmlArrayCallEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLArrayCall_Value()
  {
    return (EReference)wmlArrayCallEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLMacroDefine()
  {
    return wmlMacroDefineEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLMacroDefine_Name()
  {
    return (EAttribute)wmlMacroDefineEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroDefine_Tags()
  {
    return (EReference)wmlMacroDefineEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroDefine_Keys()
  {
    return (EReference)wmlMacroDefineEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroDefine_MacroCalls()
  {
    return (EReference)wmlMacroDefineEClass.getEStructuralFeatures().get(3);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroDefine_MacroDefines()
  {
    return (EReference)wmlMacroDefineEClass.getEStructuralFeatures().get(4);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroDefine_Textdomains()
  {
    return (EReference)wmlMacroDefineEClass.getEStructuralFeatures().get(5);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroDefine_Values()
  {
    return (EReference)wmlMacroDefineEClass.getEStructuralFeatures().get(6);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLMacroDefine_IfDefs()
  {
    return (EReference)wmlMacroDefineEClass.getEStructuralFeatures().get(7);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLMacroDefine_EndName()
  {
    return (EAttribute)wmlMacroDefineEClass.getEStructuralFeatures().get(8);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLPreprocIF()
  {
    return wmlPreprocIFEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLPreprocIF_Name()
  {
    return (EAttribute)wmlPreprocIFEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLPreprocIF_Tags()
  {
    return (EReference)wmlPreprocIFEClass.getEStructuralFeatures().get(1);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLPreprocIF_Keys()
  {
    return (EReference)wmlPreprocIFEClass.getEStructuralFeatures().get(2);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLPreprocIF_MacroCalls()
  {
    return (EReference)wmlPreprocIFEClass.getEStructuralFeatures().get(3);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLPreprocIF_MacroDefines()
  {
    return (EReference)wmlPreprocIFEClass.getEStructuralFeatures().get(4);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLPreprocIF_Textdomains()
  {
    return (EReference)wmlPreprocIFEClass.getEStructuralFeatures().get(5);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLPreprocIF_Values()
  {
    return (EReference)wmlPreprocIFEClass.getEStructuralFeatures().get(6);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWMLPreprocIF_IfDefs()
  {
    return (EReference)wmlPreprocIFEClass.getEStructuralFeatures().get(7);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLPreprocIF_Elses()
  {
    return (EAttribute)wmlPreprocIFEClass.getEStructuralFeatures().get(8);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLPreprocIF_EndName()
  {
    return (EAttribute)wmlPreprocIFEClass.getEStructuralFeatures().get(9);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLTextdomain()
  {
    return wmlTextdomainEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLTextdomain_Name()
  {
    return (EAttribute)wmlTextdomainEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getWMLValue()
  {
    return wmlValueEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getWMLValue_Value()
  {
    return (EAttribute)wmlValueEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EClass getMacroTokens()
  {
    return macroTokensEClass;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getMacroTokens_Val()
  {
    return (EAttribute)macroTokensEClass.getEStructuralFeatures().get(0);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public WmlFactory getWmlFactory()
  {
    return (WmlFactory)getEFactoryInstance();
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private boolean isCreated = false;

  /**
   * Creates the meta-model objects for the package.  This method is
   * guarded to have no affect on any invocation but its first.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void createPackageContents()
  {
    if (isCreated) return;
    isCreated = true;

    // Create classes and their features
    wmlRootEClass = createEClass(WML_ROOT);
    createEReference(wmlRootEClass, WML_ROOT__TAGS);
    createEReference(wmlRootEClass, WML_ROOT__MACRO_CALLS);
    createEReference(wmlRootEClass, WML_ROOT__MACRO_DEFINES);
    createEReference(wmlRootEClass, WML_ROOT__TEXTDOMAINS);
    createEReference(wmlRootEClass, WML_ROOT__IF_DEFS);

    wmlTagEClass = createEClass(WML_TAG);
    createEAttribute(wmlTagEClass, WML_TAG__PLUS);
    createEAttribute(wmlTagEClass, WML_TAG__NAME);
    createEReference(wmlTagEClass, WML_TAG__TAGS);
    createEReference(wmlTagEClass, WML_TAG__KEYS);
    createEReference(wmlTagEClass, WML_TAG__MACRO_CALLS);
    createEReference(wmlTagEClass, WML_TAG__MACRO_DEFINES);
    createEReference(wmlTagEClass, WML_TAG__TEXTDOMAINS);
    createEReference(wmlTagEClass, WML_TAG__IF_DEFS);
    createEAttribute(wmlTagEClass, WML_TAG__END_NAME);

    wmlKeyEClass = createEClass(WML_KEY);
    createEAttribute(wmlKeyEClass, WML_KEY__NAME);
    createEReference(wmlKeyEClass, WML_KEY__VALUE);
    createEAttribute(wmlKeyEClass, WML_KEY__EOL);

    wmlKeyValueEClass = createEClass(WML_KEY_VALUE);

    wmlMacroCallEClass = createEClass(WML_MACRO_CALL);
    createEAttribute(wmlMacroCallEClass, WML_MACRO_CALL__POINT);
    createEAttribute(wmlMacroCallEClass, WML_MACRO_CALL__RELATIVE);
    createEAttribute(wmlMacroCallEClass, WML_MACRO_CALL__NAME);
    createEReference(wmlMacroCallEClass, WML_MACRO_CALL__PARAMS);
    createEReference(wmlMacroCallEClass, WML_MACRO_CALL__EXTRA_MACROS);

    wmlLuaCodeEClass = createEClass(WML_LUA_CODE);
    createEAttribute(wmlLuaCodeEClass, WML_LUA_CODE__VALUE);

    wmlArrayCallEClass = createEClass(WML_ARRAY_CALL);
    createEReference(wmlArrayCallEClass, WML_ARRAY_CALL__VALUE);

    wmlMacroDefineEClass = createEClass(WML_MACRO_DEFINE);
    createEAttribute(wmlMacroDefineEClass, WML_MACRO_DEFINE__NAME);
    createEReference(wmlMacroDefineEClass, WML_MACRO_DEFINE__TAGS);
    createEReference(wmlMacroDefineEClass, WML_MACRO_DEFINE__KEYS);
    createEReference(wmlMacroDefineEClass, WML_MACRO_DEFINE__MACRO_CALLS);
    createEReference(wmlMacroDefineEClass, WML_MACRO_DEFINE__MACRO_DEFINES);
    createEReference(wmlMacroDefineEClass, WML_MACRO_DEFINE__TEXTDOMAINS);
    createEReference(wmlMacroDefineEClass, WML_MACRO_DEFINE__VALUES);
    createEReference(wmlMacroDefineEClass, WML_MACRO_DEFINE__IF_DEFS);
    createEAttribute(wmlMacroDefineEClass, WML_MACRO_DEFINE__END_NAME);

    wmlPreprocIFEClass = createEClass(WML_PREPROC_IF);
    createEAttribute(wmlPreprocIFEClass, WML_PREPROC_IF__NAME);
    createEReference(wmlPreprocIFEClass, WML_PREPROC_IF__TAGS);
    createEReference(wmlPreprocIFEClass, WML_PREPROC_IF__KEYS);
    createEReference(wmlPreprocIFEClass, WML_PREPROC_IF__MACRO_CALLS);
    createEReference(wmlPreprocIFEClass, WML_PREPROC_IF__MACRO_DEFINES);
    createEReference(wmlPreprocIFEClass, WML_PREPROC_IF__TEXTDOMAINS);
    createEReference(wmlPreprocIFEClass, WML_PREPROC_IF__VALUES);
    createEReference(wmlPreprocIFEClass, WML_PREPROC_IF__IF_DEFS);
    createEAttribute(wmlPreprocIFEClass, WML_PREPROC_IF__ELSES);
    createEAttribute(wmlPreprocIFEClass, WML_PREPROC_IF__END_NAME);

    wmlTextdomainEClass = createEClass(WML_TEXTDOMAIN);
    createEAttribute(wmlTextdomainEClass, WML_TEXTDOMAIN__NAME);

    wmlValueEClass = createEClass(WML_VALUE);
    createEAttribute(wmlValueEClass, WML_VALUE__VALUE);

    macroTokensEClass = createEClass(MACRO_TOKENS);
    createEAttribute(macroTokensEClass, MACRO_TOKENS__VAL);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private boolean isInitialized = false;

  /**
   * Complete the initialization of the package and its meta-model.  This
   * method is guarded to have no affect on any invocation but its first.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void initializePackageContents()
  {
    if (isInitialized) return;
    isInitialized = true;

    // Initialize package
    setName(eNAME);
    setNsPrefix(eNS_PREFIX);
    setNsURI(eNS_URI);

    // Create type parameters

    // Set bounds for type parameters

    // Add supertypes to classes
    wmlMacroCallEClass.getESuperTypes().add(this.getWMLKeyValue());
    wmlLuaCodeEClass.getESuperTypes().add(this.getWMLKeyValue());
    wmlArrayCallEClass.getESuperTypes().add(this.getWMLKeyValue());
    wmlValueEClass.getESuperTypes().add(this.getWMLKeyValue());

    // Initialize classes and features; add operations and parameters
    initEClass(wmlRootEClass, WMLRoot.class, "WMLRoot", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getWMLRoot_Tags(), this.getWMLTag(), null, "Tags", null, 0, -1, WMLRoot.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLRoot_MacroCalls(), this.getWMLMacroCall(), null, "MacroCalls", null, 0, -1, WMLRoot.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLRoot_MacroDefines(), this.getWMLMacroDefine(), null, "MacroDefines", null, 0, -1, WMLRoot.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLRoot_Textdomains(), this.getWMLTextdomain(), null, "Textdomains", null, 0, -1, WMLRoot.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLRoot_IfDefs(), this.getWMLPreprocIF(), null, "IfDefs", null, 0, -1, WMLRoot.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlTagEClass, WMLTag.class, "WMLTag", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getWMLTag_Plus(), ecorePackage.getEBoolean(), "plus", null, 0, 1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWMLTag_Name(), ecorePackage.getEString(), "name", null, 0, 1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLTag_Tags(), this.getWMLTag(), null, "Tags", null, 0, -1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLTag_Keys(), this.getWMLKey(), null, "Keys", null, 0, -1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLTag_MacroCalls(), this.getWMLMacroCall(), null, "MacroCalls", null, 0, -1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLTag_MacroDefines(), this.getWMLMacroDefine(), null, "MacroDefines", null, 0, -1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLTag_Textdomains(), this.getWMLTextdomain(), null, "Textdomains", null, 0, -1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLTag_IfDefs(), this.getWMLPreprocIF(), null, "IfDefs", null, 0, -1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWMLTag_EndName(), ecorePackage.getEString(), "endName", null, 0, 1, WMLTag.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlKeyEClass, WMLKey.class, "WMLKey", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getWMLKey_Name(), ecorePackage.getEString(), "name", null, 0, 1, WMLKey.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLKey_Value(), this.getWMLKeyValue(), null, "value", null, 0, -1, WMLKey.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWMLKey_Eol(), ecorePackage.getEString(), "eol", null, 0, 1, WMLKey.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlKeyValueEClass, WMLKeyValue.class, "WMLKeyValue", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(wmlMacroCallEClass, WMLMacroCall.class, "WMLMacroCall", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getWMLMacroCall_Point(), ecorePackage.getEBoolean(), "point", null, 0, 1, WMLMacroCall.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWMLMacroCall_Relative(), ecorePackage.getEBoolean(), "relative", null, 0, 1, WMLMacroCall.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWMLMacroCall_Name(), ecorePackage.getEString(), "name", null, 0, 1, WMLMacroCall.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroCall_Params(), ecorePackage.getEObject(), null, "params", null, 0, -1, WMLMacroCall.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroCall_ExtraMacros(), this.getWMLMacroCall(), null, "extraMacros", null, 0, -1, WMLMacroCall.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlLuaCodeEClass, WMLLuaCode.class, "WMLLuaCode", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getWMLLuaCode_Value(), ecorePackage.getEString(), "value", null, 0, 1, WMLLuaCode.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlArrayCallEClass, WMLArrayCall.class, "WMLArrayCall", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getWMLArrayCall_Value(), this.getWMLValue(), null, "value", null, 0, -1, WMLArrayCall.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlMacroDefineEClass, WMLMacroDefine.class, "WMLMacroDefine", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getWMLMacroDefine_Name(), ecorePackage.getEString(), "name", null, 0, 1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroDefine_Tags(), this.getWMLTag(), null, "Tags", null, 0, -1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroDefine_Keys(), this.getWMLKey(), null, "Keys", null, 0, -1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroDefine_MacroCalls(), this.getWMLMacroCall(), null, "MacroCalls", null, 0, -1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroDefine_MacroDefines(), this.getWMLMacroDefine(), null, "MacroDefines", null, 0, -1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroDefine_Textdomains(), this.getWMLTextdomain(), null, "Textdomains", null, 0, -1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroDefine_Values(), this.getWMLValue(), null, "Values", null, 0, -1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLMacroDefine_IfDefs(), this.getWMLPreprocIF(), null, "IfDefs", null, 0, -1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWMLMacroDefine_EndName(), ecorePackage.getEString(), "endName", null, 0, 1, WMLMacroDefine.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlPreprocIFEClass, WMLPreprocIF.class, "WMLPreprocIF", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getWMLPreprocIF_Name(), ecorePackage.getEString(), "name", null, 0, 1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLPreprocIF_Tags(), this.getWMLTag(), null, "Tags", null, 0, -1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLPreprocIF_Keys(), this.getWMLKey(), null, "Keys", null, 0, -1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLPreprocIF_MacroCalls(), this.getWMLMacroCall(), null, "MacroCalls", null, 0, -1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLPreprocIF_MacroDefines(), this.getWMLMacroDefine(), null, "MacroDefines", null, 0, -1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLPreprocIF_Textdomains(), this.getWMLTextdomain(), null, "Textdomains", null, 0, -1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLPreprocIF_Values(), this.getWMLValue(), null, "Values", null, 0, -1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWMLPreprocIF_IfDefs(), this.getWMLPreprocIF(), null, "IfDefs", null, 0, -1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWMLPreprocIF_Elses(), ecorePackage.getEString(), "Elses", null, 0, -1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWMLPreprocIF_EndName(), ecorePackage.getEString(), "endName", null, 0, 1, WMLPreprocIF.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlTextdomainEClass, WMLTextdomain.class, "WMLTextdomain", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getWMLTextdomain_Name(), ecorePackage.getEString(), "name", null, 0, 1, WMLTextdomain.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(wmlValueEClass, WMLValue.class, "WMLValue", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getWMLValue_Value(), ecorePackage.getEString(), "value", null, 0, 1, WMLValue.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(macroTokensEClass, MacroTokens.class, "MacroTokens", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getMacroTokens_Val(), ecorePackage.getEString(), "val", null, 0, 1, MacroTokens.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    // Create resource
    createResource(eNS_URI);
  }

} //WmlPackageImpl
