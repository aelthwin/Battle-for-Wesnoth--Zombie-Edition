/**
 * <copyright>
 * </copyright>
 *

 */
package org.wesnoth.wml.impl;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.wesnoth.wml.WMLKey;
import org.wesnoth.wml.WMLMacroCall;
import org.wesnoth.wml.WMLMacroDefine;
import org.wesnoth.wml.WMLPreprocIF;
import org.wesnoth.wml.WMLTag;
import org.wesnoth.wml.WMLTextdomain;
import org.wesnoth.wml.WMLValue;
import org.wesnoth.wml.WmlPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>WML Macro Define</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getName <em>Name</em>}</li>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getTags <em>Tags</em>}</li>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getKeys <em>Keys</em>}</li>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getMacroCalls <em>Macro Calls</em>}</li>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getMacroDefines <em>Macro Defines</em>}</li>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getTextdomains <em>Textdomains</em>}</li>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getValues <em>Values</em>}</li>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getIfDefs <em>If Defs</em>}</li>
 *   <li>{@link org.wesnoth.wml.impl.WMLMacroDefineImpl#getEndName <em>End Name</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class WMLMacroDefineImpl extends MinimalEObjectImpl.Container implements WMLMacroDefine
{
  /**
   * The default value of the '{@link #getName() <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getName()
   * @generated
   * @ordered
   */
  protected static final String NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getName()
   * @generated
   * @ordered
   */
  protected String name = NAME_EDEFAULT;

  /**
   * The cached value of the '{@link #getTags() <em>Tags</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getTags()
   * @generated
   * @ordered
   */
  protected EList<WMLTag> tags;

  /**
   * The cached value of the '{@link #getKeys() <em>Keys</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getKeys()
   * @generated
   * @ordered
   */
  protected EList<WMLKey> keys;

  /**
   * The cached value of the '{@link #getMacroCalls() <em>Macro Calls</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getMacroCalls()
   * @generated
   * @ordered
   */
  protected EList<WMLMacroCall> macroCalls;

  /**
   * The cached value of the '{@link #getMacroDefines() <em>Macro Defines</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getMacroDefines()
   * @generated
   * @ordered
   */
  protected EList<WMLMacroDefine> macroDefines;

  /**
   * The cached value of the '{@link #getTextdomains() <em>Textdomains</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getTextdomains()
   * @generated
   * @ordered
   */
  protected EList<WMLTextdomain> textdomains;

  /**
   * The cached value of the '{@link #getValues() <em>Values</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getValues()
   * @generated
   * @ordered
   */
  protected EList<WMLValue> values;

  /**
   * The cached value of the '{@link #getIfDefs() <em>If Defs</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getIfDefs()
   * @generated
   * @ordered
   */
  protected EList<WMLPreprocIF> ifDefs;

  /**
   * The default value of the '{@link #getEndName() <em>End Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getEndName()
   * @generated
   * @ordered
   */
  protected static final String END_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getEndName() <em>End Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getEndName()
   * @generated
   * @ordered
   */
  protected String endName = END_NAME_EDEFAULT;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected WMLMacroDefineImpl()
  {
    super();
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  protected EClass eStaticClass()
  {
    return WmlPackage.Literals.WML_MACRO_DEFINE;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getName()
  {
    return name;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setName(String newName)
  {
    String oldName = name;
    name = newName;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, WmlPackage.WML_MACRO_DEFINE__NAME, oldName, name));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<WMLTag> getTags()
  {
    if (tags == null)
    {
      tags = new EObjectContainmentEList<WMLTag>(WMLTag.class, this, WmlPackage.WML_MACRO_DEFINE__TAGS);
    }
    return tags;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<WMLKey> getKeys()
  {
    if (keys == null)
    {
      keys = new EObjectContainmentEList<WMLKey>(WMLKey.class, this, WmlPackage.WML_MACRO_DEFINE__KEYS);
    }
    return keys;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<WMLMacroCall> getMacroCalls()
  {
    if (macroCalls == null)
    {
      macroCalls = new EObjectContainmentEList<WMLMacroCall>(WMLMacroCall.class, this, WmlPackage.WML_MACRO_DEFINE__MACRO_CALLS);
    }
    return macroCalls;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<WMLMacroDefine> getMacroDefines()
  {
    if (macroDefines == null)
    {
      macroDefines = new EObjectContainmentEList<WMLMacroDefine>(WMLMacroDefine.class, this, WmlPackage.WML_MACRO_DEFINE__MACRO_DEFINES);
    }
    return macroDefines;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<WMLTextdomain> getTextdomains()
  {
    if (textdomains == null)
    {
      textdomains = new EObjectContainmentEList<WMLTextdomain>(WMLTextdomain.class, this, WmlPackage.WML_MACRO_DEFINE__TEXTDOMAINS);
    }
    return textdomains;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<WMLValue> getValues()
  {
    if (values == null)
    {
      values = new EObjectContainmentEList<WMLValue>(WMLValue.class, this, WmlPackage.WML_MACRO_DEFINE__VALUES);
    }
    return values;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EList<WMLPreprocIF> getIfDefs()
  {
    if (ifDefs == null)
    {
      ifDefs = new EObjectContainmentEList<WMLPreprocIF>(WMLPreprocIF.class, this, WmlPackage.WML_MACRO_DEFINE__IF_DEFS);
    }
    return ifDefs;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getEndName()
  {
    return endName;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public void setEndName(String newEndName)
  {
    String oldEndName = endName;
    endName = newEndName;
    if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, WmlPackage.WML_MACRO_DEFINE__END_NAME, oldEndName, endName));
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs)
  {
    switch (featureID)
    {
      case WmlPackage.WML_MACRO_DEFINE__TAGS:
        return ((InternalEList<?>)getTags()).basicRemove(otherEnd, msgs);
      case WmlPackage.WML_MACRO_DEFINE__KEYS:
        return ((InternalEList<?>)getKeys()).basicRemove(otherEnd, msgs);
      case WmlPackage.WML_MACRO_DEFINE__MACRO_CALLS:
        return ((InternalEList<?>)getMacroCalls()).basicRemove(otherEnd, msgs);
      case WmlPackage.WML_MACRO_DEFINE__MACRO_DEFINES:
        return ((InternalEList<?>)getMacroDefines()).basicRemove(otherEnd, msgs);
      case WmlPackage.WML_MACRO_DEFINE__TEXTDOMAINS:
        return ((InternalEList<?>)getTextdomains()).basicRemove(otherEnd, msgs);
      case WmlPackage.WML_MACRO_DEFINE__VALUES:
        return ((InternalEList<?>)getValues()).basicRemove(otherEnd, msgs);
      case WmlPackage.WML_MACRO_DEFINE__IF_DEFS:
        return ((InternalEList<?>)getIfDefs()).basicRemove(otherEnd, msgs);
    }
    return super.eInverseRemove(otherEnd, featureID, msgs);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public Object eGet(int featureID, boolean resolve, boolean coreType)
  {
    switch (featureID)
    {
      case WmlPackage.WML_MACRO_DEFINE__NAME:
        return getName();
      case WmlPackage.WML_MACRO_DEFINE__TAGS:
        return getTags();
      case WmlPackage.WML_MACRO_DEFINE__KEYS:
        return getKeys();
      case WmlPackage.WML_MACRO_DEFINE__MACRO_CALLS:
        return getMacroCalls();
      case WmlPackage.WML_MACRO_DEFINE__MACRO_DEFINES:
        return getMacroDefines();
      case WmlPackage.WML_MACRO_DEFINE__TEXTDOMAINS:
        return getTextdomains();
      case WmlPackage.WML_MACRO_DEFINE__VALUES:
        return getValues();
      case WmlPackage.WML_MACRO_DEFINE__IF_DEFS:
        return getIfDefs();
      case WmlPackage.WML_MACRO_DEFINE__END_NAME:
        return getEndName();
    }
    return super.eGet(featureID, resolve, coreType);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @SuppressWarnings("unchecked")
  @Override
  public void eSet(int featureID, Object newValue)
  {
    switch (featureID)
    {
      case WmlPackage.WML_MACRO_DEFINE__NAME:
        setName((String)newValue);
        return;
      case WmlPackage.WML_MACRO_DEFINE__TAGS:
        getTags().clear();
        getTags().addAll((Collection<? extends WMLTag>)newValue);
        return;
      case WmlPackage.WML_MACRO_DEFINE__KEYS:
        getKeys().clear();
        getKeys().addAll((Collection<? extends WMLKey>)newValue);
        return;
      case WmlPackage.WML_MACRO_DEFINE__MACRO_CALLS:
        getMacroCalls().clear();
        getMacroCalls().addAll((Collection<? extends WMLMacroCall>)newValue);
        return;
      case WmlPackage.WML_MACRO_DEFINE__MACRO_DEFINES:
        getMacroDefines().clear();
        getMacroDefines().addAll((Collection<? extends WMLMacroDefine>)newValue);
        return;
      case WmlPackage.WML_MACRO_DEFINE__TEXTDOMAINS:
        getTextdomains().clear();
        getTextdomains().addAll((Collection<? extends WMLTextdomain>)newValue);
        return;
      case WmlPackage.WML_MACRO_DEFINE__VALUES:
        getValues().clear();
        getValues().addAll((Collection<? extends WMLValue>)newValue);
        return;
      case WmlPackage.WML_MACRO_DEFINE__IF_DEFS:
        getIfDefs().clear();
        getIfDefs().addAll((Collection<? extends WMLPreprocIF>)newValue);
        return;
      case WmlPackage.WML_MACRO_DEFINE__END_NAME:
        setEndName((String)newValue);
        return;
    }
    super.eSet(featureID, newValue);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public void eUnset(int featureID)
  {
    switch (featureID)
    {
      case WmlPackage.WML_MACRO_DEFINE__NAME:
        setName(NAME_EDEFAULT);
        return;
      case WmlPackage.WML_MACRO_DEFINE__TAGS:
        getTags().clear();
        return;
      case WmlPackage.WML_MACRO_DEFINE__KEYS:
        getKeys().clear();
        return;
      case WmlPackage.WML_MACRO_DEFINE__MACRO_CALLS:
        getMacroCalls().clear();
        return;
      case WmlPackage.WML_MACRO_DEFINE__MACRO_DEFINES:
        getMacroDefines().clear();
        return;
      case WmlPackage.WML_MACRO_DEFINE__TEXTDOMAINS:
        getTextdomains().clear();
        return;
      case WmlPackage.WML_MACRO_DEFINE__VALUES:
        getValues().clear();
        return;
      case WmlPackage.WML_MACRO_DEFINE__IF_DEFS:
        getIfDefs().clear();
        return;
      case WmlPackage.WML_MACRO_DEFINE__END_NAME:
        setEndName(END_NAME_EDEFAULT);
        return;
    }
    super.eUnset(featureID);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public boolean eIsSet(int featureID)
  {
    switch (featureID)
    {
      case WmlPackage.WML_MACRO_DEFINE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
      case WmlPackage.WML_MACRO_DEFINE__TAGS:
        return tags != null && !tags.isEmpty();
      case WmlPackage.WML_MACRO_DEFINE__KEYS:
        return keys != null && !keys.isEmpty();
      case WmlPackage.WML_MACRO_DEFINE__MACRO_CALLS:
        return macroCalls != null && !macroCalls.isEmpty();
      case WmlPackage.WML_MACRO_DEFINE__MACRO_DEFINES:
        return macroDefines != null && !macroDefines.isEmpty();
      case WmlPackage.WML_MACRO_DEFINE__TEXTDOMAINS:
        return textdomains != null && !textdomains.isEmpty();
      case WmlPackage.WML_MACRO_DEFINE__VALUES:
        return values != null && !values.isEmpty();
      case WmlPackage.WML_MACRO_DEFINE__IF_DEFS:
        return ifDefs != null && !ifDefs.isEmpty();
      case WmlPackage.WML_MACRO_DEFINE__END_NAME:
        return END_NAME_EDEFAULT == null ? endName != null : !END_NAME_EDEFAULT.equals(endName);
    }
    return super.eIsSet(featureID);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public String toString()
  {
    if (eIsProxy()) return super.toString();

    StringBuffer result = new StringBuffer(super.toString());
    result.append(" (name: ");
    result.append(name);
    result.append(", endName: ");
    result.append(endName);
    result.append(')');
    return result.toString();
  }

} //WMLMacroDefineImpl
