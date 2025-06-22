'use client';

import { useState } from 'react';
import { Table, Badge, Button, Form } from 'react-bootstrap';

const mockPatientRecords = [
  {
    id: 'PAT-1001',
    name: 'Raj Verma',
    age: 45,
    condition: 'Heart Attack',
    admittedOn: '2025-06-14',
    status: 'Discharged',
    bill: '₹12,500',
  },
  {
    id: 'PAT-1002',
    name: 'Ritu Sharma',
    age: 30,
    condition: 'Severe Bleeding',
    admittedOn: '2025-06-19',
    status: 'Admitted',
    bill: '₹7,200',
  },
  {
    id: 'PAT-1003',
    name: 'Mohd Aamir',
    age: 60,
    condition: 'Unconsciousness',
    admittedOn: '2025-06-19',
    status: 'In Treatment',
    bill: '₹9,800',
  },
];

export default function PatientRecords() {
  const [records, setRecords] = useState(mockPatientRecords);
  const [query, setQuery] = useState('');

  const filtered = records.filter((r) =>
    r.name.toLowerCase().includes(query.toLowerCase()) ||
    r.id.toLowerCase().includes(query.toLowerCase())
  );

  return (
    <div className="container-fluid py-4">
      {/* Header */}
      <div className="d-flex justify-content-between align-items-center mb-4">
        <div>
          <h4 className="fw-bold text-success mb-1">
            <i className="bi bi-journal-medical me-2"></i>Patient Records
          </h4>
          <p className="text-muted small mb-0">
            Maintain and access historical and live records of admitted patients
          </p>
        </div>
        <Form.Control
          type="search"
          placeholder="Search by name or ID"
          style={{ maxWidth: '250px' }}
          value={query}
          onChange={(e) => setQuery(e.target.value)}
        />
      </div>

      {/* Patient Table */}
      <div className="card shadow-sm">
        <div className="card-body table-responsive">
          {filtered.length === 0 ? (
            <p className="text-center text-muted">No records found.</p>
          ) : (
            <Table hover className="align-middle small">
              <thead className="table-light">
                <tr>
                  <th>ID</th>
                  <th>Name</th>
                  <th>Age</th>
                  <th>Condition</th>
                  <th>Admitted On</th>
                  <th>Status</th>
                  <th>Bill</th>
                  <th className="text-end">Actions</th>
                </tr>
              </thead>
              <tbody>
                {filtered.map((p, i) => (
                  <tr key={i}>
                    <td>{p.id}</td>
                    <td>{p.name}</td>
                    <td>{p.age}</td>
                    <td>{p.condition}</td>
                    <td>{p.admittedOn}</td>
                    <td>
                      <Badge
                        bg={
                          p.status === 'Admitted'
                            ? 'info'
                            : p.status === 'Discharged'
                            ? 'success'
                            : 'warning'
                        }
                      >
                        {p.status}
                      </Badge>
                    </td>
                    <td>{p.bill}</td>
                    <td className="text-end">
                      <Button size="sm" variant="outline-primary">
                        <i className="bi bi-eye me-1"></i>Edit
                      </Button>{' '}
                      <Button size="sm" variant="outline-secondary">
                        <i className="bi bi-download me-1"></i>Download
                      </Button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </Table>
          )}
        </div>
      </div>
    </div>
  );
}
