/*
 *	actionTypes.h
 *
 *  Created on: January 03, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

#define OT_ACTION_MEMBER "action"

#define OT_ACTION_CMD_ServiceConnected "Service.Connected"
#define OT_ACTION_CMD_ServiceDisconnected "Service.Disconnected"
#define OT_ACTION_CMD_ShutdownRequestedByService "Service.ShutdownRequested"

#define OT_ACTION_CMD_GetDatabaseUrl "Get.DatabaseURL"
#define OT_ACTION_CMD_GetAuthorisationServerUrl "Get.AuthURL"
#define OT_ACTION_CMD_GetDBandAuthServerUrl "Get.DatabaseAuthURL"
#define OT_ACTION_CMD_GetMandatoryServices "Get.MandatoryServices"

#define OT_ACTION_CMD_Message "Message"
#define OT_ACTION_CMD_Compound "Compound"
#define OT_ACTION_CMD_ServiceClosing "Service.Closing"
#define OT_ACTION_CMD_RegisterNewService "Service.New"
#define OT_ACTION_CMD_StartNewService "Service.Start.Single"
#define OT_ACTION_CMD_StartNewServices "Service.Start.Multiple"
#define OT_ACTION_CMD_StartNewRelayService "Service.Start.Relay"
#define OT_ACTION_CMD_CreateNewSession "Session.New"
#define OT_ACTION_CMD_IsProjectOpen "Check.ProjectOpen"
#define OT_ACTION_CMD_SendBroadcastMessage "Broadcast"
#define OT_ACTION_CMD_RegisterNewSessionService "SessionService.New"
#define OT_ACTION_CMD_RegisterNewGlobalDirecotoryService "GlobalDirectoryService.New"
#define OT_ACTION_CMD_RegisterNewLocalDirecotoryService "LocalDirectoryService.New"
#define OT_ACTION_CMD_UpdateSystemLoad "SystemLoad.Update"

#define OT_ACTION_CMD_GetSessionExists "Session.GetExists"
#define OT_ACTION_CMD_GetSessionServices "Session.GetServices"
#define OT_ACTION_CMD_ShutdownSession "Session.Shutdown"
#define OT_ACTION_CMD_ShutdownSessionCompleted "Session.ShutdownCompleted"
#define OT_ACTION_CMD_ServiceFailure "Session.ServiceFailure"
#define OT_ACTION_CMD_ServiceStartupFailed "Service.Startup.Failed"
#define OT_ACTION_CMD_AddMandatoryService "Session.AddMandatory"
#define OT_ACTION_CMD_SetLoggingOptions "Logger.Set"
#define OT_ACTION_CMD_Ping "Ping"
#define OT_ACTION_CMD_Init "Initialize"
#define OT_ACTION_CMD_Run "Run"
#define OT_ACTION_CMD_Reset "Reset"
#define OT_ACTION_CMD_GetDebugInformation "Debug.Info.Get"
#define OT_ACTION_CMD_SetVisualizationModel "VisualizationModel.Set"
#define OT_ACTION_CMD_GetVisualizationModel "VisualizationModel.Get"
#define OT_ACTION_CMD_Log "Log"
#define OT_ACTION_CMD_ForceHealthCheck "Healthcheck.Force"

#define OT_ACTION_CMD_ServiceEnableDebug "Service.DebugEnable"
#define OT_ACTION_CMD_ServiceDisableDebug "Service.DebugDisable"
#define OT_ACTION_CMD_ServiceHide "Service.Hide"
#define OT_ACTION_CMD_ServiceShow "Service.Show"
#define OT_ACTION_CMD_CheckStartupCompleted "Service.CheckStartupCompleted"
#define OT_ACTION_CMD_ServiceShutdown "Shutdown"
#define OT_ACTION_CMD_ServicePreShutdown "Service.PreShutdown"
#define OT_ACTION_CMD_ServiceEmergencyShutdown "Service.EmergencyShutdown"

#define OT_ACTION_CMD_SessionDebugLogOn "SessionDebug.On"
#define OT_ACTION_CMD_SessionDebugLogOff "SessionDebug.Off"

#define OT_ACTION_CMD_PROJ_Save "Project.Save"
#define OT_ACTION_CMD_KeySequenceActivated "KeySequence.Activated"

#define OT_ACTION_CMD_MODEL_Delete "Model.Delete"
#define OT_ACTION_CMD_MODEL_SelectionChanged "Model.Event.SelectionChanged"
#define OT_ACTION_CMD_MODEL_PropertyChanged "Model.Event.PropertyChanged"
#define OT_ACTION_CMD_MODEL_ParentNeedsUpdate "Model.Event.ParentNeedsUpdate"
#define OT_ACTION_CMD_MODEL_ItemRenamed "Model.Event.ItemRenamed"
#define OT_ACTION_CMD_MODEL_GetIsModified "Model.State.Modified.Get"
#define OT_ACTION_CMD_MODEL_CommonPropertiesJSON "Model.Properties.Common.JSON"
#define OT_ACTION_CMD_MODEL_SetPropertiesFromJSON "Model.Properties.SetFromJSON"
#define OT_ACTION_CMD_MODEL_DeleteProperty "Model.Properties.Delete"
#define OT_ACTION_CMD_MODEL_GenerateEntityIDs "Model.EntityIDs.Generate"
#define OT_ACTION_CMD_MODEL_GetListOfFolderItems "Model.FolderItems.GetList"
#define OT_ACTION_CMD_MODEL_GetIDsOfFolderItemsOfType "Model.FolderIDs.GetListOfType"
#define OT_ACTION_CMD_MODEL_AddEntities "Model.Entities.Add"
#define OT_ACTION_CMD_MODEL_DeleteEntity "Model.Entities.Delete"
#define OT_ACTION_CMD_MODEL_AddGeometryOperation "Model.Geometryoperation.Add"
#define OT_ACTION_CMD_MODEL_GetEntityInformationFromID "Model.Entities.GetInformationFromID"
#define OT_ACTION_CMD_MODEL_GetEntityInformationFromName "Model.Entities.GetInformationFromName"
#define OT_ACTION_CMD_MODEL_GetEntityChildInformationFromID "Model.Entities.Childs.GetInformationFromID"
#define OT_ACTION_CMD_MODEL_GetEntityChildInformationFromName "Model.Entities.Childs.GetInformationFromName"
#define OT_ACTION_CMD_MODEL_GetSelectedEntityInformation "Model.SelectedEntities.GetEntities"
#define OT_ACTION_CMD_MODEL_GetAllGeometryEntitiesForMeshing "Model.Entities.ForMeshing"
#define OT_ACTION_CMD_MODEL_GetEntityProperties "Model.Entities.GetProperties"
#define OT_ACTION_CMD_MODEL_GetEntityPropertiesFromName "Model.Entities.GetPropertiesFromName"
#define OT_ACTION_CMD_MODEL_GetCurrentVersion "Model.Version.GetCurrent"
#define OT_ACTION_CMD_MODEL_ExecuteAction "Model.Event.ExecuteAction"
#define OT_ACTION_CMD_MODEL_ExecuteFunction "Model.Event.ExecuteFunction"
#define OT_ACTION_CMD_MODEL_EntitiesSelected "Model.Event.EntitiesSelected"
#define OT_ACTION_CMD_MODEL_GetCurrentVisModelID "Model.Visualization.GetModelID"
#define OT_ACTION_CMD_MODEL_ImportTableFile "Model.Table.ImportFromFile"
#define OT_ACTION_CMD_MODEL_CreateGeometryFromRubberbandData "Model.CreateGeometry.FromRubberbandData"
#define OT_ACTION_CMD_MODEL_UpdateVisualizationEntity "Model.Entities.UpdateVisEntity"
#define OT_ACTION_CMD_MODEL_UpdateGeometryEntity "Model.Entities.UpdateGeomEntity"
#define OT_ACTION_CMD_MODEL_ModelChangeOperationCompleted "Model.OperationCompleted"
#define OT_ACTION_CMD_MODEL_RequestUpdateVisualizationEntity "Model.Entities.RequestUpdateVisEntity"
#define OT_ACTION_CMD_MODEL_MeshingCompleted "Model.State.MeshingCompleted"
#define OT_ACTION_CMD_MODEL_QueueMessages "Model.QueueMessages"
#define OT_ACTION_CMD_MODEL_CheckParentUpdates "Model.CheckParentUpdates"
#define OT_ACTION_CMD_MODEL_ActivateVersion "Model.ActivateVersion"
#define OT_ACTION_CMD_MODEL_AddPropertiesToEntities "Model.Entities.AddProperty"
#define OT_ACTION_CMD_MODEL_GET_ENTITY_IDENTIFIER "Model.Entities.GetIdentifier"

#define OT_ACTION_CMD_UI_RegisterForModelEvents "UI.ModelView.Events.Register"
#define OT_ACTION_CMD_UI_DeregisterForModelEvents "UI.ModelView.Events.Deregister"
#define OT_ACTION_CMD_UI_DisplayMessage "UI.DisplayMessage"
#define OT_ACTION_CMD_UI_DisplayDebugMessage "UI.DisplayDebugMessage"
#define OT_ACTION_CMD_UI_ReportError "UI.ReportError"
#define OT_ACTION_CMD_UI_ReportWarning "UI.ReportWarning"
#define OT_ACTION_CMD_UI_ReportInformation "UI.ReportInformation"
#define OT_ACTION_CMD_UI_PromptInformation "UI.PromptInformation"
#define OT_ACTION_CMD_UI_PromptResponse "UI.PromptResponse"
#define OT_ACTION_CMD_UI_Lock "UI.MasterLock.Enable"
#define OT_ACTION_CMD_UI_Unlock "UI.MasterLock.Disable"
#define OT_ACTION_CMD_UI_GenerateUIDs "UI.UID.Generate"
#define OT_ACTION_CMD_UI_RequestFileForReading "UI.RequestFileForReading"
#define OT_Action_CMD_UI_StoreFileInDataBase "UI.SaveFileInDataBase"
#define OT_ACTION_CMD_UI_SelectFileForStoring "UI.SelectFileForStoring"
#define OT_ACTION_CMD_UI_SaveFileContent "UI.SaveFileContent"
#define OT_ACTION_CMD_UI_FillPropertyGrid "UI.FillPropertyGrid"
#define OT_ACTION_CMD_UI_AddMenuPage "UI.TTB.AddPage"
#define OT_ACTION_CMD_UI_AddMenuGroup "UI.TTB.AddGroup"
#define OT_ACTION_CMD_UI_AddMenuSubgroup "UI.TTB.AddSubgroup"
#define OT_ACTION_CMD_UI_AddMenuButton "UI.TTB.AddToolButton"
#define OT_ACTION_CMD_UI_AddMenuCheckbox "UI.TTB.AddToolCheckBox"
#define OT_ACTION_CMD_UI_AddMenuLineEdit "UI.TTB.AddMenuLineEdit"
#define OT_ACTION_CMD_UI_ActivateToolbarTab "UI.TTB.ActivateTab"
#define OT_ACTION_CMD_UI_AddShortcut "UI.Shortcut.Add"
#define OT_ACTION_CMD_UI_SetCheckboxValues "UI.TTB.SetCheckBoxValues"
#define OT_ACTION_CMD_UI_SetLineEditValues "UI.TTB.SetLineEditValues"
#define OT_ACTION_CMD_UI_SwitchMenuTab "UI.TTB.SwitchTab"
#define OT_ACTION_CMD_UI_RemoveElements "UI.RemoveElements"
#define OT_ACTION_CMD_UI_EnableDisableControls "UI.EnableDisableControls"
#define OT_ACTION_CMD_UI_OBJ_SetToolTip "UI.OBJ.SetToolTip"
#define OT_ACTION_CMD_UI_SetProgressVisibility "UI.Progressbar.SetVisibility"
#define OT_ACTION_CMD_UI_SetProgressbarValue "UI.Progressbar.SetValue"
#define OT_ACTION_CMD_UI_AddSettingsData "UI.AddSettingsData"
#define OT_ACTION_CMD_UI_SettingsItemChanged "UI.SettingsData.ItemChanged"
#define OT_ACTION_CMD_UI_ContextMenuItemClicked "UI.ContextMenu.Item.Clicked"
#define OT_ACTION_CMD_UI_ContextMenuItemCheckedChanged "UI.ContextMenu.Item.CheckedChanged"
#define OT_ACTION_CMD_UI_AddIconSearchPath "UI.IconManager.AddSearchPath"
#define OT_ACTION_CMD_UI_VIEW_SetVersionGraph "UI.SetVersionGraph"
#define OT_ACTION_CMD_UI_VIEW_SetVersionGraphActive "UI.SetVersionGraph.Active"
#define OT_ACTION_CMD_UI_VIEW_RemoveVersionGraphVersions "UI.RemoveVersionGraph.Versions"
#define OT_ACTION_CMD_UI_VIEW_AddAndActivateNewVersionGraphVersion "UI.AddActivateGraph.Version"

#define OT_ACTION_CMD_UI_AddPluginSearchPath "UI.Plugin.AddSearchPath"
#define OT_ACTION_CMD_UI_RequestPlugin "UI.Plugin.RequestConnection"
#define OT_ACTION_CMD_UI_RequestPluginSuccess "UI.Plugin.RequestConnection.Success"
#define OT_ACTION_CMD_UI_PluginMessage "UI.Plugin.SendMessage"

#define OT_ACTION_CMD_UI_MODEL_Create "UI.Model.Create"

#define OT_ACTION_CMD_UI_VIEW_Create "UI.View.Create"
#define OT_ACTION_CMD_UI_VIEW_Reset "UI.View.Reset"
#define OT_ACTION_CMD_UI_VIEW_ClearSelection "UI.View.ClearSelection"
#define OT_ACTION_CMD_UI_VIEW_RefreshSelection "UI.View.RefreshSelection"
#define OT_ACTION_CMD_UI_VIEW_SelectObject "UI.View.SelectObject"
#define OT_ACTION_CMD_UI_VIEW_Refresh "UI.View.Refresh"
#define OT_ACTION_CMD_UI_VIEW_AddNodeFromFacetData "UI.View.Nodes.AddFromFacetData"
#define OT_ACTION_CMD_UI_VIEW_AddNodeFromDataBase "UI.View.Nodes.AddFromDataBase"
#define OT_ACTION_CMD_UI_VIEW_AddContainerNode "UI.View.Node.AddContainer"
#define OT_ACTION_CMD_UI_VIEW_AddVis2D3DNode "UI.View.Node.AddVis2D3D"
#define OT_ACTION_CMD_UI_VIEW_UpdateVis2D3DNode "UI.View.Node.UpdateVis2D3D"
#define OT_ACTION_CMD_UI_VIEW_OBJ_UpdateColor "UI.VIEW.OBJ.UpdateColor"
#define OT_ACTION_CMD_UI_VIEW_OBJ_UpdateFacetsFromDataBase "UI.VIEW.OBJ.UpdateFacetsFromDataBase"
#define OT_ACTION_CMD_UI_VIEW_OBJ_RemoveShapes "UI.VIEW.OBJ.Shapes.Remove"
#define OT_ACTION_CMD_UI_VIEW_OBJ_TreeStateRecording "UI.VIEW.OBJ.Tree.StateRecording"
#define OT_ACTION_CMD_UI_VIEW_OBJ_SetShapeVisibility "UI.VIEW.OBJ.Shapes.SetVisibility"
#define OT_ACTION_CMD_UI_VIEW_OBJ_HideEntities "UI.VIEW.OBJ.Shapes.HideEntities"
#define OT_ACTION_CMD_UI_VIEW_OBJ_ShowBranch "UI.VIEW.OBJ.Shapes.ShowBranch"
#define OT_ACTION_CMD_UI_VIEW_OBJ_HideBranch "UI.VIEW.OBJ.Shapes.HideBranch"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNode "UI.VIEW.OBJ.AddAnnotatioNode"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNodeFromDatabase "UI.VIEW.OBJ.AddAnnotatioNodeFromDatabase"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshNodeFromFacetDatabase "UI.VIEW.OBJ.AddMeshNodeFromFacetDatabase"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshItemFromFacetDatabase "UI.VIEW.OBJ.AddMeshItemFromFacetDatabase"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshNode "UI.VIEW.OBJ.AddCartesianMeshNode"
#define OT_ACTION_CMD_UI_VIEW_OBJ_CartesianMeshNodeShowLines "UI.VIEW.OBJ.CartesianMeshNodeShowLines"
#define OT_ACTION_CMD_UI_VIEW_OBJ_TetMeshNodeTetEdges "UI.VIEW.OBJ.TetMeshNodeShowTetEdges"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshItem "UI.VIEW.OBJ.AddCartesianMeshItem"
#define OT_ACTION_CMD_UI_VIEW_OBJ_UpdateMeshColor "UI.VIEW.OBJ.UpdateMeshColor"
#define OT_ACTION_CMD_UI_VIEW_EnterEntitySelectionMode "UI.VIEW.EnterEntitySelectionMode"
#define OT_ACTION_CMD_UI_VIEW_SetModifiedState "UI.View.SetModifiedState"
#define OT_ACTION_CMD_UI_VIEW_SetEntityName "UI.VIEW.Entity.Name.Set"
#define OT_ACTION_CMD_UI_VIEW_RenameEntityName "UI.VIEW.Entity.Name.Rename"
#define OT_ACTION_CMD_UI_VIEW_OBJ_Result1DPropsChanged "UI.VIEW.OBJ.Result1DPropertiesChanged"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddPlot1D "UI.VIEW.OBJ.AddPlot1D"
#define OT_ACTION_CMD_UI_VIEW_OBJ_Plot1DPropsChanged "UI.VIEW.OBJ.Plot1DPropertiesChanged"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddText "UI.VIEW.OBJ.AddText"
#define OT_ACTION_CMD_UI_VIEW_OBJ_AddTable "UI.VIEW.OBJ.AddTable"
#define OT_ACTION_CMD_UI_VIEW_CreateRubberband "UI.VIEW.CreateRubberband"
#define OT_ACTION_CMD_UI_VIEW_OBJ_GetTableSelection "UI.VIEW.GetTableSelection"
#define OT_ACTION_CMD_UI_VIEW_OBJ_ShowTable "UI.VIEW.ShowTable"
#define OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddRow "UI.VIEW.ADDROW"
#define OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteRow "UI.VIEW.DELETEROW"
#define OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddColumn "UI.VIEW.ADDCOLUMN"
#define OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteColumn "UI.VIEW.DELETECOLUMN"

#define OT_ACTION_CMD_UI_VIEW_OBJ_ColourSelection "UI.VIEW.ColourSelection"
#define OT_ACTION_CMD_UI_VIEW_OBJ_SelectRanges "UI.VIEW.SelectRanges"

#define OT_ACTION_CMD_PYTHON_EXECUTE_STRINGS "Python.Execute.String"
#define OT_ACTION_CMD_PYTHON_Request_Initialization "Python.Request.Initialize"
#define OT_ACTION_CMD_PYTHON_Initialization "Python.Initialize"


#define OT_ACTION_CMD_UI_BLOCKEDITOR_CreateEmptyBlockEditor "UI.BlockEditor.CreateEmpty"
//#define OT_ACTION_CMD_UI_BLOCKEDITOR_CreateBlockEditor "UI.BlockEditor.Create"
#define OT_ACTION_CMD_UI_BLOCKEDITOR_BlockDropped "UI.BlockEditor.Block.Dropped"
#define OT_ACTION_CMD_UI_BLOCKEDITOR_BlockRemoved "UI.BlockEditor.Block.Removed"
#define OT_ACTION_CMD_UI_BLOCKEDITOR_ConnectionDropped "UI.BlockEditor.Connection.Dropped"
#define OT_ACTION_CMD_UI_BLOCKEDITOR_ConnectionRemoved "UI.BlockEditor.Connection.Removed"

#define OT_ACTION_CMD_CARTESIANMESH_Run "CARTESIANMESH.Run"

#define OT_ACTION_PARAM_BASETYPE_UID "BaseTypes.UID"
#define OT_ACTION_PARAM_BASETYPE_Bool "BaseTypes.Boolean"
#define OT_ACTION_PARAM_BASETYPE_List "BaseTypes.List"
#define OT_ACTION_PARAM_BASETYPE_Action "BaseTypes.Action"
#define OT_ACTION_PARAM_BASETYPE_Props "BaseTypes.Props"

#define OT_ACTION_PARAM_DATABASE_URL "Database.URL"
#define OT_ACTION_PARAM_PROCESS_ID "PROCESS.ID"

#define OT_ACTION_PARAM_UI_ToolBarTabOrder "UI.TTB.TabOrder"
#define OT_ACTION_PARAM_UI_CONTROL_TabName "UI.TTB.TabName"
#define OT_ACTION_PARAM_UI_CONTROL_PageName "UI.TTB.Page.Name"
#define OT_ACTION_PARAM_UI_CONTROL_GroupName "UI.TTB.Group.Name"
#define OT_ACTION_PARAM_UI_CONTROL_SubgroupName "UI.TTB.Subgroup.Name"
#define OT_ACTION_PARAM_UI_CONTROL_ObjectName "UI.Object.Name"
#define OT_ACTION_PARAM_UI_CONTROL_ObjectNames "UI.Object.Names"
#define OT_ACTION_PARAM_UI_CONTROL_ObjectText "UI.Object.Text"
#define OT_ACTION_PARAM_UI_CONTROL_ObjectType "UI.Object.Type"
#define OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText "UI.Label.Text"
#define OT_ACTION_PARAM_UI_CONTROL_SelectionType "UI.Control.SelectionType"

#define OT_ACTION_PARAM_UI_TREE_Name "UI.Tree.Name"
#define OT_ACTION_PARAM_UI_TREE_IconSize "UI.Tree.IconSize"
#define OT_ACTION_PARAM_UI_TREE_IconItemVisible "UI.Tree.ItemVisibleIcon"
#define OT_ACTION_PARAM_UI_TREE_IconItemHidden "UI.Tree.ItemHiddenIcon"

#define OT_ACTION_CMD_UI_Freeze3DView "UI.View.Freeze"
#define OT_ACTION_PARAM_UI_DisabledControlsList "UI.Object.DisabledList"
#define OT_ACTION_PARAM_UI_EnabledControlsList "UI.Object.EnabledList"
#define OT_ACTION_PARAM_UI_CONTROL_CheckedState "UI.Cfg.Checked"
#define OT_ACTION_PARAM_UI_CONTROL_ErrorState "UI.Cfg.Error"
#define OT_ACTION_PARAM_UI_CONTROL_VisibleState "UI.Cfg.Visible"
#define OT_ACTION_PARAM_UI_CONTROL_ContinuousState "UI.Cfg.Continuous"
#define OT_ACTION_PARAM_UI_CONTROL_IconName "UI.Cfg.IconName"
#define OT_ACTION_PARAM_UI_CONTROL_IconFolder "UI.Cfg.IconFolder"
#define OT_ACTION_PARAM_UI_CONTROL_PropertyGridSettingsJSON "UI.PropertyGrid.Settings.JSON"
#define OT_ACTION_PARAM_UI_CONTROL_ContextMenu "UI.Cfg.ContextMenu"
#define OT_ACTION_PARAM_UI_CONTROL_ContextMenuName "UI.Cfg.ContextMenu.Name"
#define OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName "UI.Cfg.ContextMenu.Item.Name"
#define OT_ACTION_PARAM_UI_CONTROL_Checkable "UI.Cfg.ContextMenu.Item.Checkable"
#define OT_ACTION_PARAM_UI_CONTROL_BOOLEAN_STATE "UI.Cfg.BooleanState"
#define OT_ACTION_PARAM_UI_DIALOG_TITLE "UI.Dialog.Title"
#define OT_ACTION_PARAM_UI_UID "UI.UID"
#define OT_ACTION_PARAM_UI_KeySequence "UI.KeySequence"
#define OT_ACTION_PARAM_UI_GRAPH_VERSION "UI.Graph.Version"
#define OT_ACTION_PARAM_UI_GRAPH_PARENT "UI.Graph.Parent"
#define OT_ACTION_PARAM_UI_GRAPH_DESCRIPTION "UI.Graph.Description"
#define OT_ACTION_PARAM_UI_GRAPH_ACTIVE "UI.Graph.Active"
#define OT_ACTION_PARAM_UI_GRAPH_BRANCH "UI.Graph.Branch"

#define OT_ACTION_PARAM_UI_PLUGIN_ACTION_MEMBER "UI.Plugin.Action"
#define OT_ACTION_PARAM_UI_PLUGIN_NAME "UI.Plugin.Name"
#define OT_ACTION_PARAM_UI_PLUGIN_PATH "UI.Plugin.Path"
#define OT_ACTION_PARAM_UI_PLUGIN_UID "UI.Plugin.UID"

#define OT_ACTION_PARAM_FILE_Mode "File.Mode"
#define OT_ACTION_PARAM_FILE_Mask "File.Mask"
#define OT_ACTION_PARAM_FILE_Name "File.Name"
#define OT_ACTION_PARAM_FILE_TAKEN_NAMES "File.TakenNames"
#define OT_ACTION_PARAM_FILE_Content "File.Content"
#define OT_ACTION_PARAM_FILE_Content_UncompressedDataLength "File.Content.DataLength.Uncompressed"
#define OT_ACTION_PARAM_FILE_OriginalName "File.OriginalName"
#define OT_ACTION_PARAM_FILE_LoadContent "File.LoadContent"
#define OT_ACTION_PARAM_SENDER_URL "Sender.URL"

#define OT_ACTION_PARAM_LOG "LogMessage"

#define OT_ACTION_PARAM_PREFETCH_Documents "Prefetch.Documents"
#define OT_ACTION_PARAM_PREFETCH_ID "Prefetch.ID"
#define OT_ACTION_PARAM_PREFETCH_Version "Prefetch.Version"

#define OT_ACTION_PARAM_SERVICE_URL "Service.URL"
#define OT_ACTION_PARAM_SERVICE_NAME "Service.Name"
#define OT_ACTION_PARAM_SERVICE_TYPE "Service.Type"
#define OT_ACTION_PARAM_SERVICE_ID "Service.ID"
#define OT_ACTION_PARAM_SERVICE_DBURL "Service.DbUrl"
#define OT_ACTION_PARAM_SERVICE_AUTHURL "Service.AuthUrl"

#define OT_ACTION_PARAM_SESSION_ID "Session.ID"
#define OT_ACTION_PARAM_SESSION_TYPE "Session.Type"
#define OT_ACTION_PARAM_SESSION_SERVICES "Session.Services"
#define OT_ACTION_PARAM_SESSION_EXISTS "Session.Exists"

#define OT_ACTION_PARAM_SITE_ID "Site.ID"
#define OT_ACTION_PARAM_SESSION_SERVICE_URL "SessionService.URL"
#define OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "GlobalDirectoryService.URL"
#define OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL "LocalDirectoryService.URL"

#define OT_ACTION_PARAM_START_RELAY "Start.Relay"
#define OT_ACTION_PARAM_SUPPORTED_SERVICES "Services.Supported"

#define OT_ACTION_PARAM_COLLECTION_NAME "Collection.Name"
#define OT_ACTION_PARAM_PROJECT_NAME "Project.Name"
#define OT_ACTION_PARAM_USER_NAME "User.Name"

#define OT_ACTION_PARAM_MODEL_ActionName "Model.Event.Actions.Name"
#define OT_ACTION_PARAM_MODEL_ID "Model.ID"
#define OT_ACTION_PARAM_MODEL_REPLYTO "Model.ReplyTo"
#define OT_ACTION_PARAM_MODEL_EntityID "Model.Entity.ID"
#define OT_ACTION_PARAM_MODEL_EntityVersion "Model.Entity.Version"
#define OT_ACTION_PARAM_MODEL_EntityID_Brep "Model.Entity.BrepID"
#define OT_ACTION_PARAM_MODEL_EntityID_Facets "Model.Entity.FacetsID"
#define OT_ACTION_PARAM_MODEL_EntityVersion_Brep "Model.Entity.BrepVersion"
#define OT_ACTION_PARAM_MODEL_EntityVersion_Facets "Model.Entity.FacetsVersion"
#define OT_ACTION_PARAM_MODEL_OverrideGeometry "Model.Entity.OverrideGeometry"
#define OT_ACTION_PARAM_MODEL_ENTITY_IDENTIFIER_AMOUNT "Model.Entity.Identifier.Amount"
#define OT_ACTION_PARAM_MODEL_Description "Model.Description"
#define OT_ACTION_PARAM_MODEL_NewProperties "Model.Entity.NewProperties"
#define OT_ACTION_PARAM_VIEW_ID "View.ID"
#define OT_ACTION_PARAM_MODEL_FunctionName "Model.Event.Functions.Name"
#define OT_ACTION_PARAM_MODEL_ITM_Description "Model.Item.Description"
#define OT_ACTION_PARAM_MODEL_PropertyList "Model.Properties.List"
#define OT_ACTION_PARAM_MODEL_Update "Model.Action.Update"
#define OT_ACTION_PARAM_MODEL_ItemsVisible "Model.Action.ItemsVisible"
#define OT_ACTION_PARAM_MODEL_EntityIDList "Model.Entities.IDList"
#define OT_ACTION_PARAM_MODEL_EntityName "Model.Entities.Name"
#define OT_ACTION_PARAM_MODEL_EntityNameList "Model.Entities.NameList"
#define OT_ACTION_PARAM_MODEL_DataID "Model.Entities.DataID"
#define OT_ACTION_PARAM_MODEL_DataVersion "Model.Entities.DataVersion"
#define OT_ACTION_PARAM_MODEL_SourceID "Model.Entities.SourceID"
#define OT_ACTION_PARAM_MODEL_SourceVersion "Model.Entities.SourceVersion"
#define OT_ACTION_PARAM_MODEL_MeshID "Model.Entities.MeshID"
#define OT_ACTION_PARAM_MODEL_MeshVersion "Model.Entities.MeshVersion"
#define OT_ACTION_PARAM_MODEL_SaveModel "Model.SaveModel"
#define OT_ACTION_PARAM_MODEL_EntityVersionList "Model.Entities.VersionList"
#define OT_ACTION_PARAM_MODEL_BrepVersionList "Model.Brep.VersionList"
#define OT_ACTION_PARAM_MODEL_EntityInfoID "Model.EntityInfo.ID"
#define OT_ACTION_PARAM_MODEL_EntityInfoVersion "Model.EntityInfo.Version"
#define OT_ACTION_PARAM_MODEL_EntityParentList "Model.Entities.ParentList"
#define OT_ACTION_PARAM_MODEL_EntityTypeList "Model.Entities.TypeList"
#define OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB "Model.Item.Surface.Color"
#define OT_ACTION_PARAM_MODEL_ITM_EdgeRGB "Model.Item.Edge.Color"
#define OT_ACTION_PARAM_MODEL_ITM_MaterialType "Model.Item.MaterialType"
#define OT_ACTION_PARAM_MODEL_ITM_TextureType "Model.Item.Texture.Type"
#define OT_ACTION_PARAM_MODEL_ITM_TextureReflective "Model.Item.Texture.Reflective"
#define OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling "Model.Item.BackFace.Culling"
#define OT_ACTION_PARAM_MODEL_ITM_OffsetFactor "Model.Item.OffsetFactor"
#define OT_ACTION_PARAM_MODEL_ITM_IsEditable "Model.Item.IsEditable"
#define OT_ACTION_PARAM_MODEL_ITM_IsHidden "Model.Item.IsHidden"
#define OT_ACTION_PARAM_MODEL_ITM_SelectChildren "Model.Item.SelectChilds"
#define OT_ACTION_PARAM_MODEL_ITM_ManageParentVis "Model.Item.ManageParentVis"
#define OT_ACTION_PARAM_MODEL_ITM_ManageChildVis "Model.Item.ManageChildVis"
#define OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected "Model.Item.ShowWhenSelected"
#define OT_ACTION_PARAM_MODEL_ITM_Transformation "Model.Item.Transformation"
#define OT_ACTION_PARAM_MODEL_ITM_Nodes "Model.Item.Nodes"
#define OT_ACTION_PARAM_MODEL_ITM_FacesList "Model.Item.FacesList"
#define OT_ACTION_PARAM_MODEL_ITM_Triangles "Model.Item.Triangles"
#define OT_ACTION_PARAM_MODEL_ITM_Edges "Model.Item.Edges"
#define OT_ACTION_PARAM_MODEL_ITM_Errors "Model.Item.Errors"
#define OT_ACTION_PARAM_MODEL_ITM_ID "Model.Item.ID"
#define OT_ACTION_PARAM_MODEL_ITM_ID_Hidden "Model.Item.ID.Hidden"
#define OT_ACTION_PARAM_MODEL_ITM_ID_Visible "Model.Item.ID.Visible"
#define OT_ACTION_PARAM_MODEL_ITM_BRANCH "Model.Item.Branch"
#define OT_ACTION_PARAM_MODEL_ITM_Name "Model.Item.Name"
#define OT_ACTION_PARAM_MODEL_ITM_Version "Model.Item.Version"
#define OT_ACTION_PARAM_MODEL_ITM_Points "Model.Item.Points"
#define OT_ACTION_PARAM_MODEL_ITM_Points_Colors "Model.Item.Points.Colors"
#define OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p1 "Model.Item.Points.Triangle.P1"
#define OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p2 "Model.Item.Points.Triangle.P2"
#define OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p3 "Model.Item.Points.Triangle.P3"
#define OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_Color "Model.Item.Points.Triangle.Color"
#define OT_ACTION_PARAM_MODEL_ITM_Selection_AllowMultipleSelection "Model.Item.Selection.AllowMultipleSelection"
#define OT_ACTION_PARAM_MODEL_ITM_Selection_Filter "Model.Item.Selection.Filter"
#define OT_ACTION_PARAM_MODEL_ITM_Selection_Action "Model.Item.Selection.Action"
#define OT_ACTION_PARAM_MODEL_ITM_Selection_Message "Model.Item.Selection.Message"
#define OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames "Model.Item.Selection.OptionNames"
#define OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues "Model.Item.Selection.OptionValues"
#define OT_ACTION_PARAM_MODEL_EDGE_COLOR_R "Model.Edge.Color.R"
#define OT_ACTION_PARAM_MODEL_EDGE_COLOR_G "Model.Edge.Color.G"
#define OT_ACTION_PARAM_MODEL_EDGE_COLOR_B "Model.Edge.Color.B"
#define OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_R "Model.MeshLine.Color.R"
#define OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_G "Model.MeshLine.Color.G"
#define OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_B "Model.MeshLine.Color.B"
#define OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES "Model.Tet.DisplayEdges"
#define OT_ACTION_PARAM_MODEL_ModifiedState "Model.ModifiedState"
#define OT_ACTION_PARAM_MODEL_SelectedEntityIDs "Model.Selection.EntityIDList"
#define OT_ACTION_PARAM_MODEL_SelectedVisibleEntityIDs "Model.Selection.VisibleEntityIDList"
#define OT_ACTION_PARAM_MODEL_SelectionAction "Model.Selection.Action"
#define OT_ACTION_PARAM_MODEL_SelectionInfo "Model.Selection.Info"
#define OT_ACTION_PARAM_MODEL_TETEDGES_ID "Model.TetEdges.ID"
#define OT_ACTION_PARAM_MODEL_TETEDGES_Version "Model.TetEdges.Version"
#define OT_ACTION_PARAM_MODEL_TopologyEntityIDList "Model.Topo.EntityID"
#define OT_ACTION_PARAM_MODEL_TopologyEntityVersionList "Model.Topo.EntityVersion"
#define OT_ACTION_PARAM_MODEL_TopologyEntityForceShowList "Model.Topo.EntityForceShow"
#define OT_ACTION_PARAM_MODEL_DataEntityIDList "Model.Data.EntityID"
#define OT_ACTION_PARAM_MODEL_DataEntityVersionList "Model.Data.EntityVersion"
#define OT_ACTION_PARAM_MODEL_DataEntityParentList "Model.Data.EntityParent"
#define OT_ACTION_PARAM_MODEL_EntityChildrenList "Model.Entity.Children"
#define OT_ACTION_PARAM_MODEL_State "Model.Entity.State"
#define OT_ACTION_PARAM_MODEL_Version "Model.Version"
#define OT_ACTION_PARAM_MODEL_JSON "Model.JSON"

#define OT_ACTION_PARAM_MESH_CartesianCoordX "Mesh.Cartesian.X"
#define OT_ACTION_PARAM_MESH_CartesianCoordY "Mesh.Cartesian.Y"
#define OT_ACTION_PARAM_MESH_CartesianCoordZ "Mesh.Cartesian.Z"
#define OT_ACTION_PARAM_MESH_ShowMeshLines "Mesh.ShowMeshLines"
#define OT_ACTION_PARAM_MESH_ITEM_COLOR_R "Mesh.Item.Color.R"
#define OT_ACTION_PARAM_MESH_ITEM_COLOR_G "Mesh.Item.Color.G"
#define OT_ACTION_PARAM_MESH_ITEM_COLOR_B "Mesh.Item.Color.B"
#define OT_ACTION_PARAM_MESH_NODE_ID "Mesh.Node.ID"
#define OT_ACTION_PARAM_MESH_NODE_VERSION "Mesh.Node.Version"

#define OT_ACTION_PARAM_VIEW1D_Title "View1D.Title"
#define OT_ACTION_PARAM_VIEW1D_PlotType "View1D.PlotType"
#define OT_ACTION_PARAM_VIEW1D_PlotQuantity "View1D.PlotQuantity"
#define OT_ACTION_PARAM_VIEW1D_CurveIDs "View1D.CurveIDs"
#define OT_ACTION_PARAM_VIEW1D_CurveVersions "View1D.CurveVersions"
#define OT_ACTION_PARAM_VIEW1D_CurveNames "View1D.CurveNames"
#define OT_ACTION_PARAM_VIEW1D_Legend "View1D.Legend"
#define OT_ACTION_PARAM_VIEW1D_Grid "View1D.Grid"
#define OT_ACTION_PARAM_VIEW1D_GridColorR "View1D.GridColorR"
#define OT_ACTION_PARAM_VIEW1D_GridColorG "View1D.GridColorG"
#define OT_ACTION_PARAM_VIEW1D_GridColorB "View1D.GridColorB"
#define OT_ACTION_PARAM_VIEW1D_LogscaleX "View1D.LogscaleX"
#define OT_ACTION_PARAM_VIEW1D_LogscaleY "View1D.LogscaleY"
#define OT_ACTION_PARAM_VIEW1D_AutoscaleX "View1D.AutoscaleX"
#define OT_ACTION_PARAM_VIEW1D_AutoscaleY "View1D.AutoscaleY"
#define OT_ACTION_PARAM_VIEW1D_Xmin "View1D.Xmin"
#define OT_ACTION_PARAM_VIEW1D_Xmax "View1D.Xmax"
#define OT_ACTION_PARAM_VIEW1D_Ymin "View1D.Ymin"
#define OT_ACTION_PARAM_VIEW1D_Ymax "View1D.Ymax"

#define OT_ACTION_PARAM_VIEW_RUBBERBAND_Note "View.Rubberband.Note"
#define OT_ACTION_PARAM_VIEW_RUBBERBAND_Document "View.Rubberband.Document"
#define OT_ACTION_PARAM_VIEW_RUBBERBAND_PointDocument "View.Rubberband.PointDocument"
#define OT_ACTION_PARAM_VIEW_RUBBERBAND_Transform "View.Rubberband.Transform"

#define OT_ACTION_PARAM_TEXT_ID "Text.Entity.ID"
#define OT_ACTION_PARAM_TEXT_VERSION "Text.Entity.Version"

#define OT_ACTION_PARAM_COLOUR_BACKGROUND "Colour.Background"

#define OT_ACTION_PARAM_SETTINGS_DataName "SettingsData.Dataset.Name"
#define OT_ACTION_PARAM_SETTINGS_Version "SettingsData.Version"
#define OT_ACTION_PARAM_SETTINGS_Groups "SettingsData.Groups"
#define OT_ACTION_PARAM_SETTINGS_Item "SettingsData.Item"
#define OT_ACTION_PARAM_SETTINGS_LogicalName "SettingsData.LogicalName"
#define OT_ACTION_PARAM_SETTINGS_Items "SettingsData.Items"
#define OT_ACTION_PARAM_SETTINGS_Name "SettingsData.Name"
#define OT_ACTION_PARAM_SETTINGS_Title "SettingsData.Title"
#define OT_ACTION_PARAM_SETTINGS_Type "SettingsData.Type"
#define OT_ACTION_PARAM_SETTINGS_PossibleValue "SettingsData.PossibleValue"
#define OT_ACTION_PARAM_SETTINGS_Value "SettingsData.Value"
#define OT_ACTION_PARAM_SETTINGS_ValueMax "SettingsData.ValueMax"
#define OT_ACTION_PARAM_SETTINGS_ValueMin "SettingsData.ValueMin"
#define OT_ACTION_PARAM_SETTINGS_Decimals "SettingsData.Decimals"
#define OT_ACTION_PARAM_SETTINGS_ValueR "SettingsData.ValueR"
#define OT_ACTION_PARAM_SETTINGS_ValueG "SettingsData.ValueG"
#define OT_ACTION_PARAM_SETTINGS_ValueB "SettingsData.ValueB"
#define OT_ACTION_PARAM_SETTINGS_ValueA "SettingsData.ValueA"
#define OT_ACTION_PARAM_SETTINGS_Filter "SettingsData.Filter"
#define OT_ACTION_PARAM_SETTINGS_InputHint "SettingsData.InputHint"
#define OT_ACTION_PARAM_SETTINGS_IsVisible "SettingsData.IsVisible"
#define OT_ACTION_PARAM_SETTINGS_Description "SettingsData.Description"

#define OT_ACTION_PARAM_CARTESIANMESH_EntityIDList "CartesianMesh.EntityList"
#define OT_ACTION_PARAM_CARTESIANMESH_MeshEntityID "CartesianMesh.MeshEntityID"
#define OT_ACTION_PARAM_CARTESIANMESH_ModelStateVersion "CartesianMesh.ModelVersion"

#define OT_ACTION_PARAM_BLOCKEDITOR_ConfigurationPackage "BlockEditor.ConfigurationPackage"
#define OT_ACTION_PARAM_BLOCKEDITOR_Categories "BlockEditor.Categories"
#define OT_ACTION_PARAM_BLOCKEDITOR_EditorName "BlockEditor.Name"
#define OT_ACTION_PARAM_BLOCKEDITOR_EditorTitle "BlockEditor.Title"
#define OT_ACTION_PARAM_BLOCKEDITOR_BlockName "BlockEditor.Block.Name"
#define OT_ACTION_PARAM_BLOCKEDITOR_BlockId "BlockEditor.Block.ID"
#define OT_ACTION_PARAM_BLOCKEDITOR_BlockFrom "BlockEditor.BlockFrom.ID"
#define OT_ACTION_PARAM_BLOCKEDITOR_ConnectorFrom "BlockEditor.ConnectorFrom.ID"
#define OT_ACTION_PARAM_BLOCKEDITOR_BlockTo "BlockEditor.BlockTo.ID"
#define OT_ACTION_PARAM_BLOCKEDITOR_ConnectorTo "BlockEditor.ConnectorTo.ID"

#define OT_ACTION_PARAM_MESSAGE "Msg"
#define OT_ACTION_PARAM_ICON "Icon"
#define OT_ACTION_PARAM_OPTIONS "Options"
#define OT_ACTION_PARAM_RESPONSE "Response"
#define OT_ACTION_PARAM_SENDER "Sender"
#define OT_ACTION_PARAM_ANSWER "Answer"
#define OT_ACTION_PARAM_PARAMETER1 "Parameter1"
#define OT_ACTION_PARAM_PORT "Port"

#define OT_ACTION_PARAM_COUNT "Count"
#define OT_ACTION_PARAM_PERCENT "Percentage"
#define OT_ACTION_PARAM_PATH_FROM "Path.From"
#define OT_ACTION_PARAM_PATH_To "Path.To"
#define OT_ACTION_PARAM_Folder "Folder"
#define OT_ACTION_PARAM_Recursive "Recursive"
#define OT_ACTION_PARAM_Type "Type"
#define OT_ACTION_PARAM_NAME "Name"
#define OT_ACTION_PARAM_QUEUE_FLAG "Queue.Messages.Flag"
#define OT_ACTION_PARAM_Filter "Filter"

#define OT_ACTION_PARAM_WebsocketURL "Websocket.URL"

#define OT_ACTION_PARAM_RegisterForModelEvents "ModelEvents.Register"
#define OT_ACTION_PARAM_ElementLockTypes "UI.Element.LockTypes"

#define OT_ACTION_PARAM_SESSION_LIST "Session.List"

#define OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory "System.PhysicalMemory.Total"
#define OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory "System.PhysicalMemory.Available"
#define OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory "System.VirtualMemory.Total"
#define OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory "System.VirtualMemory.Available"

#define OT_ACTION_VALUE_FILE_Mode_Name "File.Mode:Name"
#define OT_ACTION_VALUE_FILE_Mode_Content "File.Mode:Content"

#define OT_ACTION_VALUE_UI_ObjectType_ToolButton "UI.ObjectType.Type.ToolButton"
#define OT_ACTION_VALUE_UI_ObjectType_SubGroup "UI.ObjectType.Type.SubGroup"
#define OT_ACTION_VALUE_UI_ObjectType_Group "UI.ObjectType.Type.Group"
#define OT_ACTION_VALUE_UI_ObjectType_Menu "UI.ObjectType.Type.Menu"

#define OT_ACTION_RETURN_VALUE_OK "OK"
#define OT_ACTION_RETURN_VALUE_FAILED "FAILED"
#define OT_ACTION_RETURN_VALUE_TRUE "True"
#define OT_ACTION_RETURN_VALUE_FALSE "False"

#define OT_INFO_SERVICE_TYPE_MODEL "Model"
#define OT_INFO_SERVICE_TYPE_UI "UI"
#define OT_INFO_SERVICE_TYPE_LOGGER "LoggerService"
#define OT_INFO_SERVICE_TYPE_AuthorisationService "AuthorisationService"
#define OT_INFO_SERVICE_TYPE_PHREEC "PHREECService"
#define OT_INFO_SERVICE_TYPE_FITTD "FITTDService"
#define OT_INFO_SERVICE_TYPE_KRIGING "KrigingService"
#define OT_INFO_SERVICE_TYPE_SessionService "SessionService"
#define OT_INFO_SERVICE_TYPE_GlobalSessionService "GlobalSessionService"
#define OT_INFO_SERVICE_TYPE_ModelingService "ModelingService"
#define OT_INFO_SERVICE_TYPE_VisualizationService "VisualizationService"
#define OT_INFO_SERVICE_TYPE_CartesianMeshService "CartesianMeshService"
#define OT_INFO_SERVICE_TYPE_TetMeshService "TetMeshService"
#define OT_INFO_SERVICE_TYPE_LocalDirectoryService "LocalDirectoryService"
#define OT_INFO_SERVICE_TYPE_GlobalDirectoryService "GlobalDirectoryService"
#define OT_INFO_SERVICE_TYPE_RelayService "RelayService"
#define OT_INFO_SERVICE_TYPE_ImportParameterizedDataService "ImportParameterizedDataService"
#define OT_INFO_SERVICE_TYPE_GETDP "GetDPService"
#define OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE "PythonExecutionService"
#define OT_INFO_SERVICE_TYPE_BlockEditorService "BlockEditorService"


#define OT_INFO_MESSAGE_LogIn "UI.LogIn"

#define OT_PARAM_SETTINGS_USERCOLLECTION "Settings.UserCollection"

#define OT_ACTION_RETURN_INDICATOR_Error "ERROR: "
#define OT_ACTION_RETURN_INDICATOR_Warning "WARNING: "
#define OT_ACTION_RETURN_UnknownError OT_ACTION_RETURN_INDICATOR_Error "Unknown error"
#define OT_ACTION_RETURN_UnknownAction OT_ACTION_RETURN_INDICATOR_Error "Unknown action"

#define OT_ACTION_IF_RESPONSE_ERROR(___response) if (___response.rfind(OT_ACTION_RETURN_INDICATOR_Error, 0) != std::string::npos)
#define OT_ACTION_IF_RESPONSE_WARNING(___response) if (___response.rfind(OT_ACTION_RETURN_INDICATOR_Warning, 0) != std::string::npos)


// -------------- Authorisation Service --------------------

#define OT_ACTION_PING "Ping"
// User Functionalities
#define OT_ACTION_LOGIN "Login"
#define OT_ACTION_LOGIN_ADMIN "LoginAdmin"
#define OT_ACTION_REGISTER "Register"
#define OT_ACTION_GET_USER_DATA "GetUserData"
#define OT_ACTION_GET_ALL_USERS "GetAllUsers"
#define OT_ACTION_GET_ALL_USER_COUNT "GetAllUserCount"
#define OT_ACTION_CHANGE_USER_USERNAME "ChangeUserUsername"
#define OT_ACTION_CHANGE_USER_PASSWORD "ChangeUserPassword"
#define OT_ACTION_CHANGE_USERNAME "ChangeUsername"
#define OT_ACTION_CHANGE_PASSWORD "ChangePassword"
#define OT_ACTION_DELETE_USER "DeleteUser"
#define OT_ACTION_AUTH_ERROR "error"
#define OT_ACTION_AUTH_SUCCESS "successful"
#define OT_ACTION_SETTINGS_COLLECTION_NAME "settingsCollectionName"

// todo: ot param auth username exists twice: USERNAME and USER_NAME
#define OT_PARAM_AUTH_USERNAME "Username"
#define OT_PARAM_USERNAME_TO_DELETE "UsernameToDelete"
#define OT_PARAM_AUTH_LOGGED_IN_USERNAME "LoggedInUsername"
#define OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD "LoggedInUserPassword"
#define OT_PARAM_AUTH_OLD_USERNAME "OldUsername"
#define OT_PARAM_AUTH_NEW_USERNAME "NewUsername"
#define OT_PARAM_AUTH_PASSWORD "Password"
#define OT_PARAM_AUTH_NEW_PASSWORD "NewPassword"
#define OT_PARAM_AUTH_ENCRYPTED_PASSWORD "EncryptedPassword"

// Group Functionalities
#define OT_ACTION_CREATE_GROUP "CreateGroup"
#define OT_ACTION_GET_GROUP_DATA "GetGroupData"
#define OT_ACTION_GET_ALL_USER_GROUPS "GetAllUserGroups"
#define OT_ACTION_GET_ALL_GROUPS "GetAllGroups"
#define OT_ACTION_GET_ALL_GROUP_COUNT "GetAllGroupCount"
#define OT_ACTION_CHANGE_GROUP_NAME "ChangeGroupName"
#define OT_ACTION_CHANGE_GROUP_OWNER "ChangeGroupOwner"
#define OT_ACTION_ADD_USER_TO_GROUP "AddUserToGroup"
#define OT_ACTION_REMOVE_USER_FROM_GROUP "RemoveUserFromGroup"
#define OT_ACTION_REMOVE_GROUP "RemoveGroup"


#define OT_PARAM_AUTH_GROUP_NAME "GroupName"
#define OT_PARAM_AUTH_NEW_GROUP_NAME "NewGroupName"
#define OT_PARAM_AUTH_GROUP_OWNER_USER_USERNAME "GroupOwnerUsername"
#define OT_PARAM_AUTH_GROUP_OWNER_NEW_USER_USERNAME "NewGroupOwnerUsername"


// Project Functionalities
#define OT_ACTION_CREATE_PROJECT "CreateProject"
#define OT_ACTION_GET_PROJECT_DATA "GetProjectData"
#define OT_ACTION_GET_ALL_PROJECT_OWNERS "GetAllProjectOwners"
#define OT_ACTION_GET_ALL_PROJECTS "GetAllProjects"
#define OT_ACTION_GET_ALL_USER_PROJECTS "GetAllUserProjects"
#define OT_ACTION_GET_ALL_PROJECT_COUNT "GetAllProjectCount"
#define OT_ACTION_GET_ALL_GROUP_PROJECTS "GetAllGroupProjects"
#define OT_ACTION_CHANGE_PROJECT_NAME "ChangeProjectName"
#define OT_ACTION_CHANGE_PROJECT_OWNER "ChangeProjectOwner"
#define OT_ACTION_ADD_GROUP_TO_PROJECT "AddGroupToProject"
#define OT_ACTION_REMOVE_GROUP_FROM_PROJECT "RemoveGroupFromProject"
#define OT_ACTION_REMOVE_PROJECT "RemoveProject"


#define OT_PARAM_AUTH_PROJECT_NAME "ProjectName"
#define OT_PARAM_AUTH_PROJECT_NAMES "ProjectNames"
#define OT_PARAM_AUTH_NEW_PROJECT_NAME "NewProjectName"
#define OT_PARAM_AUTH_NEW_PROJECT_OWNER "NewProjectOwner"
#define OT_PARAM_AUTH_PROJECT_ID "ProjectId"
#define OT_PARAM_AUTH_PROJECT_FILTER "ProjectFilter"
#define OT_PARAM_AUTH_PROJECT_LIMIT "ProjectLimit"
#define OT_PARAM_AUTH_PROJECT_COLLECTION "collectionName"
#define OT_PARAM_AUTH_NAME "name"
#define OT_PARAM_AUTH_OWNER "creatingUser"
#define OT_PARAM_AUTH_GROUP "groupName"
#define OT_PARAM_AUTH_GROUPOWNER "ownerUsername"
#define OT_PARAM_AUTH_USER_NAME "username"



