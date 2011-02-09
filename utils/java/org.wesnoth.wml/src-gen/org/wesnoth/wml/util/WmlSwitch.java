/**
 * <copyright>
 * </copyright>
 *

 */
package org.wesnoth.wml.util;

import java.util.List;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

import org.wesnoth.wml.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy.
 * It supports the call {@link #doSwitch(EObject) doSwitch(object)}
 * to invoke the <code>caseXXX</code> method for each class of the model,
 * starting with the actual class of the object
 * and proceeding up the inheritance hierarchy
 * until a non-null result is returned,
 * which is the result of the switch.
 * <!-- end-user-doc -->
 * @see org.wesnoth.wml.WmlPackage
 * @generated
 */
public class WmlSwitch<T>
{
  /**
   * The cached model package
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected static WmlPackage modelPackage;

  /**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public WmlSwitch()
  {
    if (modelPackage == null)
    {
      modelPackage = WmlPackage.eINSTANCE;
    }
  }

  /**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
  public T doSwitch(EObject theEObject)
  {
    return doSwitch(theEObject.eClass(), theEObject);
  }

  /**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
  protected T doSwitch(EClass theEClass, EObject theEObject)
  {
    if (theEClass.eContainer() == modelPackage)
    {
      return doSwitch(theEClass.getClassifierID(), theEObject);
    }
    else
    {
      List<EClass> eSuperTypes = theEClass.getESuperTypes();
      return
        eSuperTypes.isEmpty() ?
          defaultCase(theEObject) :
          doSwitch(eSuperTypes.get(0), theEObject);
    }
  }

  /**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
  protected T doSwitch(int classifierID, EObject theEObject)
  {
    switch (classifierID)
    {
      case WmlPackage.WML_ROOT:
      {
        WMLRoot wmlRoot = (WMLRoot)theEObject;
        T result = caseWMLRoot(wmlRoot);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_TAG:
      {
        WMLTag wmlTag = (WMLTag)theEObject;
        T result = caseWMLTag(wmlTag);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_KEY:
      {
        WMLKey wmlKey = (WMLKey)theEObject;
        T result = caseWMLKey(wmlKey);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_KEY_VALUE:
      {
        WMLKeyValue wmlKeyValue = (WMLKeyValue)theEObject;
        T result = caseWMLKeyValue(wmlKeyValue);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_MACRO_CALL:
      {
        WMLMacroCall wmlMacroCall = (WMLMacroCall)theEObject;
        T result = caseWMLMacroCall(wmlMacroCall);
        if (result == null) result = caseWMLKeyValue(wmlMacroCall);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_LUA_CODE:
      {
        WMLLuaCode wmlLuaCode = (WMLLuaCode)theEObject;
        T result = caseWMLLuaCode(wmlLuaCode);
        if (result == null) result = caseWMLKeyValue(wmlLuaCode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_ARRAY_CALL:
      {
        WMLArrayCall wmlArrayCall = (WMLArrayCall)theEObject;
        T result = caseWMLArrayCall(wmlArrayCall);
        if (result == null) result = caseWMLKeyValue(wmlArrayCall);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_MACRO_DEFINE:
      {
        WMLMacroDefine wmlMacroDefine = (WMLMacroDefine)theEObject;
        T result = caseWMLMacroDefine(wmlMacroDefine);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_PREPROC_IF:
      {
        WMLPreprocIF wmlPreprocIF = (WMLPreprocIF)theEObject;
        T result = caseWMLPreprocIF(wmlPreprocIF);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_TEXTDOMAIN:
      {
        WMLTextdomain wmlTextdomain = (WMLTextdomain)theEObject;
        T result = caseWMLTextdomain(wmlTextdomain);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.WML_VALUE:
      {
        WMLValue wmlValue = (WMLValue)theEObject;
        T result = caseWMLValue(wmlValue);
        if (result == null) result = caseWMLKeyValue(wmlValue);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case WmlPackage.MACRO_TOKENS:
      {
        MacroTokens macroTokens = (MacroTokens)theEObject;
        T result = caseMacroTokens(macroTokens);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      default: return defaultCase(theEObject);
    }
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Root</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Root</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLRoot(WMLRoot object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Tag</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Tag</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLTag(WMLTag object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Key</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Key</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLKey(WMLKey object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Key Value</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Key Value</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLKeyValue(WMLKeyValue object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Macro Call</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Macro Call</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLMacroCall(WMLMacroCall object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Lua Code</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Lua Code</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLLuaCode(WMLLuaCode object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Array Call</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Array Call</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLArrayCall(WMLArrayCall object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Macro Define</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Macro Define</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLMacroDefine(WMLMacroDefine object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Preproc IF</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Preproc IF</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLPreprocIF(WMLPreprocIF object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Textdomain</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Textdomain</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLTextdomain(WMLTextdomain object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>WML Value</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>WML Value</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWMLValue(WMLValue object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Macro Tokens</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Macro Tokens</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseMacroTokens(MacroTokens object)
  {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>EObject</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null;
   * returning a non-null result will terminate the switch, but this is the last case anyway.
   * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>EObject</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject)
   * @generated
   */
  public T defaultCase(EObject object)
  {
    return null;
  }

} //WmlSwitch
