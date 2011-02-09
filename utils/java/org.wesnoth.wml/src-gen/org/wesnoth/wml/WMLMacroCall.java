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
 * A representation of the model object '<em><b>WML Macro Call</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.wesnoth.wml.WMLMacroCall#isPoint <em>Point</em>}</li>
 *   <li>{@link org.wesnoth.wml.WMLMacroCall#isRelative <em>Relative</em>}</li>
 *   <li>{@link org.wesnoth.wml.WMLMacroCall#getName <em>Name</em>}</li>
 *   <li>{@link org.wesnoth.wml.WMLMacroCall#getParams <em>Params</em>}</li>
 *   <li>{@link org.wesnoth.wml.WMLMacroCall#getExtraMacros <em>Extra Macros</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.wesnoth.wml.WmlPackage#getWMLMacroCall()
 * @model
 * @generated
 */
public interface WMLMacroCall extends WMLKeyValue
{
  /**
   * Returns the value of the '<em><b>Point</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Point</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Point</em>' attribute.
   * @see #setPoint(boolean)
   * @see org.wesnoth.wml.WmlPackage#getWMLMacroCall_Point()
   * @model
   * @generated
   */
  boolean isPoint();

  /**
   * Sets the value of the '{@link org.wesnoth.wml.WMLMacroCall#isPoint <em>Point</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Point</em>' attribute.
   * @see #isPoint()
   * @generated
   */
  void setPoint(boolean value);

  /**
   * Returns the value of the '<em><b>Relative</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Relative</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Relative</em>' attribute.
   * @see #setRelative(boolean)
   * @see org.wesnoth.wml.WmlPackage#getWMLMacroCall_Relative()
   * @model
   * @generated
   */
  boolean isRelative();

  /**
   * Sets the value of the '{@link org.wesnoth.wml.WMLMacroCall#isRelative <em>Relative</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Relative</em>' attribute.
   * @see #isRelative()
   * @generated
   */
  void setRelative(boolean value);

  /**
   * Returns the value of the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Name</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Name</em>' attribute.
   * @see #setName(String)
   * @see org.wesnoth.wml.WmlPackage#getWMLMacroCall_Name()
   * @model
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link org.wesnoth.wml.WMLMacroCall#getName <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Params</b></em>' containment reference list.
   * The list contents are of type {@link org.eclipse.emf.ecore.EObject}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Params</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Params</em>' containment reference list.
   * @see org.wesnoth.wml.WmlPackage#getWMLMacroCall_Params()
   * @model containment="true"
   * @generated
   */
  EList<EObject> getParams();

  /**
   * Returns the value of the '<em><b>Extra Macros</b></em>' containment reference list.
   * The list contents are of type {@link org.wesnoth.wml.WMLMacroCall}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Extra Macros</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Extra Macros</em>' containment reference list.
   * @see org.wesnoth.wml.WmlPackage#getWMLMacroCall_ExtraMacros()
   * @model containment="true"
   * @generated
   */
  EList<WMLMacroCall> getExtraMacros();

} // WMLMacroCall
