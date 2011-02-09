/**
 * <copyright>
 * </copyright>
 *

 */
package org.wesnoth.wml;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>WML Root</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.wesnoth.wml.WMLRoot#getTags <em>Tags</em>}</li>
 *   <li>{@link org.wesnoth.wml.WMLRoot#getMacroCalls <em>Macro Calls</em>}</li>
 *   <li>{@link org.wesnoth.wml.WMLRoot#getMacroDefines <em>Macro Defines</em>}</li>
 *   <li>{@link org.wesnoth.wml.WMLRoot#getTextdomains <em>Textdomains</em>}</li>
 *   <li>{@link org.wesnoth.wml.WMLRoot#getIfDefs <em>If Defs</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.wesnoth.wml.WmlPackage#getWMLRoot()
 * @model
 * @generated
 */
public interface WMLRoot extends EObject
{
  /**
   * Returns the value of the '<em><b>Tags</b></em>' containment reference list.
   * The list contents are of type {@link org.wesnoth.wml.WMLTag}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Tags</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Tags</em>' containment reference list.
   * @see org.wesnoth.wml.WmlPackage#getWMLRoot_Tags()
   * @model containment="true"
   * @generated
   */
  EList<WMLTag> getTags();

  /**
   * Returns the value of the '<em><b>Macro Calls</b></em>' containment reference list.
   * The list contents are of type {@link org.wesnoth.wml.WMLMacroCall}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Macro Calls</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Macro Calls</em>' containment reference list.
   * @see org.wesnoth.wml.WmlPackage#getWMLRoot_MacroCalls()
   * @model containment="true"
   * @generated
   */
  EList<WMLMacroCall> getMacroCalls();

  /**
   * Returns the value of the '<em><b>Macro Defines</b></em>' containment reference list.
   * The list contents are of type {@link org.wesnoth.wml.WMLMacroDefine}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Macro Defines</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Macro Defines</em>' containment reference list.
   * @see org.wesnoth.wml.WmlPackage#getWMLRoot_MacroDefines()
   * @model containment="true"
   * @generated
   */
  EList<WMLMacroDefine> getMacroDefines();

  /**
   * Returns the value of the '<em><b>Textdomains</b></em>' containment reference list.
   * The list contents are of type {@link org.wesnoth.wml.WMLTextdomain}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Textdomains</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Textdomains</em>' containment reference list.
   * @see org.wesnoth.wml.WmlPackage#getWMLRoot_Textdomains()
   * @model containment="true"
   * @generated
   */
  EList<WMLTextdomain> getTextdomains();

  /**
   * Returns the value of the '<em><b>If Defs</b></em>' containment reference list.
   * The list contents are of type {@link org.wesnoth.wml.WMLPreprocIF}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>If Defs</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>If Defs</em>' containment reference list.
   * @see org.wesnoth.wml.WmlPackage#getWMLRoot_IfDefs()
   * @model containment="true"
   * @generated
   */
  EList<WMLPreprocIF> getIfDefs();

} // WMLRoot
