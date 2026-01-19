# Inventory System

A click-to-pickup inventory system made with Unreal Engine 5.4, built on top of the Third Person Template.

## Features

- **Click-to-Pickup**: Click on the level objects to add them to inventory
- **Tag Filtering**: Prevent the player from falling by picking up floors and walls 
- **Toggle UI**: Show or hide the inventory interface
- **Capacity Management**: Promote resourcefulness by not exceeding the inventory's capacity
- **Line Trace Detection**: Convert mouse clicks into world-space coordinates for object detection

## Custom Properties

- `ToggleAction` and `ClickAction` for binding input actions
- `Inventory` widget reference
- `InventoryTagsToIgnore` array for filtering non-pickable objects

## Custom Functions

- `DisplayInventory()` - Creates inventory widget and enables mouse cursor functionality
- `OnToggle()` - Toggles inventory UI visibility
- `OnClick()` - Objects added to inventory on click

## Template Base

Built on top of Unreal Engine 5 Third Person Template.

## Usage

- Click only on level objects to add them to inventory
- Press **Tab** key to show or hide inventory
- Objects tagged as "Floor" and "Wall" cannot be picked up 

## Requirements

- Unreal Engine 5
- Enhanced Input System
- C++ development tools
