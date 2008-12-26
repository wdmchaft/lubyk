/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_SELECTEDITEMSET_JUCEHEADER__
#define __JUCE_SELECTEDITEMSET_JUCEHEADER__

#include "../events/juce_ChangeBroadcaster.h"


//==============================================================================
/** Manages a list of selectable items.

    Use one of these to keep a track of things that the user has highlighted, like
    icons or things in a list.

    The class is templated so that you can use it to hold either a set of pointers
    to objects, or a set of ID numbers or handles, for cases where each item may
    not always have a corresponding object.

    To be informed when items are selected/deselected, register a ChangeListener with
    this object.

    @see SelectableObject
*/
template <class SelectableItemType>
class JUCE_API  SelectedItemSet   : public ChangeBroadcaster
{
public:
    //==============================================================================
    /** Creates an empty set. */
    SelectedItemSet()
    {
    }

    /** Creates a set based on an array of items. */
    SelectedItemSet (const Array <SelectableItemType>& items)
        : selectedItems (items)
    {
    }

    /** Creates a copy of another set. */
    SelectedItemSet (const SelectedItemSet& other)
        : selectedItems (other.selectedItems)
    {
    }

    /** Creates a copy of another set. */
    const SelectedItemSet& operator= (const SelectedItemSet& other)
    {
        if (selectedItems != other.selectedItems)
        {
            selectedItems = other.selectedItems;
            changed();
        }

        return *this;
    }

    /** Destructor. */
    ~SelectedItemSet()
    {
    }

    //==============================================================================
    /** Clears any other currently selected items, and selects this item.

        If this item is already the only thing selected, no change notification
        will be sent out.

        @see addToSelection, addToSelectionBasedOnModifiers
    */
    void selectOnly (SelectableItemType item)
    {
        if (isSelected (item))
        {
            for (int i = selectedItems.size(); --i >= 0;)
            {
                if (selectedItems.getUnchecked(i) != item)
                {
                    deselect (selectedItems.getUnchecked(i));
                    i = jmin (i, selectedItems.size());
                }
            }
        }
        else
        {
            deselectAll();
            changed();

            selectedItems.add (item);
            itemSelected (item);
        }
    }

    /** Selects an item.

        If the item is already selected, no change notification will be sent out.

        @see selectOnly, addToSelectionBasedOnModifiers
    */
    void addToSelection (SelectableItemType item)
    {
        if (! isSelected (item))
        {
            changed();

            selectedItems.add (item);
            itemSelected (item);
        }
    }

    /** Selects or deselects an item.

        This will use the modifier keys to decide whether to deselect other items
        first.

        So if the shift key is held down, the item will be added without deselecting
        anything (same as calling addToSelection() )

        If no modifiers are down, the current selection will be cleared first (same
        as calling selectOnly() )

        If the ctrl (or command on the Mac) key is held down, the item will be toggled -
        so it'll be added to the set unless it's already there, in which case it'll be
        deselected.

        If the items that you're selecting can also be dragged, you may need to use the
        addToSelectionOnMouseDown() and addToSelectionOnMouseUp() calls to handle the
        subtleties of this kind of usage.

        @see selectOnly, addToSelection, addToSelectionOnMouseDown, addToSelectionOnMouseUp
    */
    void addToSelectionBasedOnModifiers (SelectableItemType item,
                                         const ModifierKeys& modifiers)
    {
        if (modifiers.isShiftDown())
        {
            addToSelection (item);
        }
        else if (modifiers.isCommandDown())
        {
            if (isSelected (item))
                deselect (item);
            else
                addToSelection (item);
        }
        else
        {
            selectOnly (item);
        }
    }

    /** Selects or deselects items that can also be dragged, based on a mouse-down event.

        If you call addToSelectionOnMouseDown() at the start of your mouseDown event,
        and then call addToSelectionOnMouseUp() at the end of your mouseUp event, this
        makes it easy to handle multiple-selection of sets of objects that can also
        be dragged.

        For example, if you have several items already selected, and you click on
        one of them (without dragging), then you'd expect this to deselect the other, and
        just select the item you clicked on. But if you had clicked on this item and
        dragged it, you'd have expected them all to stay selected.

        When you call this method, you'll need to store the boolean result, because the
        addToSelectionOnMouseUp() method will need to be know this value.

        @see addToSelectionOnMouseUp, addToSelectionBasedOnModifiers
    */
    bool addToSelectionOnMouseDown (SelectableItemType item,
                                    const ModifierKeys& modifiers)
    {
        if (isSelected (item))
        {
            return ! modifiers.isPopupMenu();
        }
        else
        {
            addToSelectionBasedOnModifiers (item, modifiers);
            return false;
        }
    }

    /** Selects or deselects items that can also be dragged, based on a mouse-up event.

        Call this during a mouseUp callback, when you have previously called the
        addToSelectionOnMouseDown() method during your mouseDown event.

        See addToSelectionOnMouseDown() for more info

        @param item             the item to select (or deselect)
        @param modifiers        the modifiers from the mouse-up event
        @param wasItemDragged   true if your item was dragged during the mouse click
        @param resultOfMouseDownSelectMethod    this is the boolean return value that came
                                back from the addToSelectionOnMouseDown() call that you
                                should have made during the matching mouseDown event
    */
    void addToSelectionOnMouseUp (SelectableItemType item,
                                  const ModifierKeys& modifiers,
                                  const bool wasItemDragged,
                                  const bool resultOfMouseDownSelectMethod)
    {
        if (resultOfMouseDownSelectMethod && ! wasItemDragged)
            addToSelectionBasedOnModifiers (item, modifiers);
    }

    /** Deselects an item. */
    void deselect (SelectableItemType item)
    {
        const int i = selectedItems.indexOf (item);

        if (i >= 0)
        {
            changed();
            itemDeselected (selectedItems.remove (i));
        }
    }

    /** Deselects all items. */
    void deselectAll()
    {
        if (selectedItems.size() > 0)
        {
            changed();

            for (int i = selectedItems.size(); --i >= 0;)
            {
                itemDeselected (selectedItems.remove (i));
                i = jmin (i, selectedItems.size());
            }
        }
    }

    //==============================================================================
    /** Returns the number of currently selected items.

        @see getSelectedItem
    */
    int getNumSelected() const throw()
    {
        return selectedItems.size();
    }

    /** Returns one of the currently selected items.

        Returns 0 if the index is out-of-range.

        @see getNumSelected
    */
    SelectableItemType getSelectedItem (const int index) const throw()
    {
        return selectedItems [index];
    }

    /** True if this item is currently selected. */
    bool isSelected (const SelectableItemType item) const throw()
    {
        return selectedItems.contains (item);
    }

    const Array <SelectableItemType>& getItemArray() const throw()          { return selectedItems; }

    //==============================================================================
    /** Can be overridden to do special handling when an item is selected.

        For example, if the item is an object, you might want to call it and tell
        it that it's being selected.
    */
    virtual void itemSelected (SelectableItemType item)                     {}

    /** Can be overridden to do special handling when an item is deselected.

        For example, if the item is an object, you might want to call it and tell
        it that it's being deselected.
    */
    virtual void itemDeselected (SelectableItemType item)                   {}

    /** Used internally, but can be called to force a change message to be sent to the ChangeListeners.
    */
    void changed (const bool synchronous = false)
    {
        if (synchronous)
            sendSynchronousChangeMessage (this);
        else
            sendChangeMessage (this);
    }

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    Array <SelectableItemType> selectedItems;
};





#endif   // __JUCE_SELECTEDITEMSET_JUCEHEADER__