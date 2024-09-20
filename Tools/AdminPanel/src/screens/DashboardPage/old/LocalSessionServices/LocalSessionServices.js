import React from 'react';
import DataGrid from '../../../../components/DataGrid/DataGrid';

const LocalSessionServices = ({ services }) => {
    const columns = [
        { field: 'user', headerName: 'User', width: 200 },
        {
            field: 'accessedLDS',
            headerName: 'Accessed LDS',
            width: 400,
            renderCell: (params) => (
                <ul>
                    {params.value.map((lds, index) => (
                        <li key={index}>{lds}</li>
                    ))}
                </ul>
            )
        }
    ];

    return (
        <div className="card">
            <div className="card-header">Local Session Services</div>
            <div className="card-body">
                {services.map((service, index) => (
                    <DataGrid
                        key={index}
                        list={service.localSessionServices}
                        columnHeader={columns}
                        title={`Local Session Service #${index + 1}`}
                    />
                ))}
            </div>
        </div>
    );
};

export default LocalSessionServices;
