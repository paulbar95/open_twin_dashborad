import "./DataGrid.scss";
import React, { useState, useEffect } from "react";
import { useContext } from "react";
import { Link, useNavigate } from "react-router-dom";
import { useTranslation } from "react-i18next";

import { DataGrid as MuiDataGrid } from "@mui/x-data-grid";
import { makeStyles, createStyles } from "@material-ui/styles";
import { Button, Alert } from "@mui/material";
import { UserContext } from "../context/userDetails";

import GetAllProjectCount from "../../services/api/AuthorisationService/ProjectActions/GetAllProjectCount";

import QuickFilter from "../QuickFilter/QuickFilter";

const DataGrid = ({ list, columnHeader, title, handleOpenModal, ...props }) => {
  const { t } = useTranslation();
  const navigate = useNavigate();
  const { setUserDetails } = useContext(UserContext);

  const [showAlert, setShowAlert] = useState("");

  useEffect(() => {
    if (title === "Projects") {
      GetAllProjectCount()
        .then((response) => {
          const number = response;
          if (number > 6) {
            setShowAlert(t("dataGrid:projectLisAlert"));
          }
        })
        .catch((response) => {
          console.log("GetAllProjectCount", response.description);
          navigate("/error");
        });
    }
  }, [navigate, t, title]);

  const actionUserColumn = [
    {
      field: "action",
      headerName: "Change Actions",
      width: 200,
      renderCell: (rowData) => (
        <>
          <div className="cellAction">
            <Link to="/users/changeUsername" style={{ textDecoration: "none" }}>
              <Button size="small" onClick={() => setUserDetails(rowData.row.username)}>
                {t("dataGrid:changeUsernameButton")}
              </Button>
            </Link>
          </div>
          <div className="cellAction">
            <Link to="/users/changePassword" style={{ textDecoration: "none" }}>
              <Button size="small" onClick={() => setUserDetails(rowData.row.username)}>
                {t("dataGrid:changePasswordButton")}
              </Button>
            </Link>
          </div>
        </>
      ),
    },
  ];

  const actionGroupColumn = [
    {
      field: "action",
      headerName: "Action",
      width: 200,
      renderCell: (rowData) => (
        <div className="cellAction">
          <Link to="/groups/ChangeData" style={{ textDecoration: "none" }}>
            <Button size="small" onClick={() => setUserDetails(rowData.row.groupName)}>
              {t("dataGrid:changeGroupName")}
            </Button>
          </Link>
        </div>
      ),
    },
  ];

  const actionProjectColumn = [
    {
      field: "action",
      headerName: "Action",
      width: 200,
      renderCell: (rowData) => (
        <div className="cellAction">
          <Link to="/projects/ChangeData" style={{ textDecoration: "none" }}>
            <Button size="small" onClick={() => setUserDetails(rowData.row.name)}>
              {t("dataGrid:changeProjectName")}
            </Button>
          </Link>
        </div>
      ),
    },
  ];

  const actionLocalDirectoryServiceColumn = [
    {
      field: "action",
      headerName: "Action",
      width: 200,
      renderCell: (rowData) => (
        <div className="cellAction">
          <Button size="small" variant="outlined" onClick={() => handleOpenModal(rowData.row.localSessionServices)}>
            View Services
          </Button>
        </div>
      ),
    },
  ];

  const actionColum =
    title === "Users"
      ? columnHeader.concat(actionUserColumn)
      : title === "Groups"
        ? columnHeader.concat(actionGroupColumn)
        : title === "Projects"
          ? columnHeader.concat(actionProjectColumn)
          : title === "Local Directory Services"
            ? columnHeader.concat(actionLocalDirectoryServiceColumn)
            : columnHeader;

  var arrIds = [];
  const setArrIds = (ids) => {
    arrIds = ids;
    props.onItemSelect(arrIds);
  };

  const useStyles = makeStyles((theme) =>
    createStyles({
      root: {
        "& .MuiDataGrid-columnHeaders": {
          backgroundColor: "rgba(171, 181, 214, 1)",
          fontSize: 16,
        },
        "& .MuiDataGrid-virtualScrollerRenderZone": {
          "& .MuiDataGrid-row": {
            "&:nth-child(2n)": { backgroundColor: "rgba(235, 235, 235, .7)" },
          },
        },
      },
    })
  );
  const classes = useStyles();

  console.log("pageSize:", props.pageSize); // Hinzugefügt für Debugging
  console.log("pageSize:", props.rowsPerPageOptions); // Hinzugefügt für Debugging
  const [paginationModel, setPaginationModel] = React.useState({
    pageSize: 10,
    page: 0,
  });
  return (
    <div className="dataGrid">
      <div className="dataGridTitle">
        {props.title}
        {props.children}
      </div>
      {title === "Projects" && (
        <>
          <QuickFilter input={props.input} handleFilter={props.handleFilter} />
          {props.numberOfProject >= 10 && (
            <Alert severity="info">{showAlert}</Alert>
          )}
        </>
      )}
      <MuiDataGrid
        paginationModel={paginationModel}
        onPaginationModelChange={setPaginationModel}
        className={classes.root}
        rows={list}
        rowHeight={50}
        columns={actionColum}
        pageSize={10}
        pageSizeOptions={[10, 15, 50, 100]}
        checkboxSelection
        onSelectionModelChange={(ids) => {
          setArrIds(ids);
        }}
      />
    </div>
  );
};

export default DataGrid;
